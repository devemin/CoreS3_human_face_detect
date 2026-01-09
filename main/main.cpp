// Copyright (c) devemin. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.
//

#include <M5Unified.h>

#include "esp_log.h"
#include "bsp/esp-bsp.h"
#include "esp_camera.h"

//#include "dog_detect.hpp"
#include "human_face_detect.hpp"
//#include "dl_image_jpeg.hpp"


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

const char *TAG = "my_detect_app";
M5Canvas canvas(&M5.Lcd);
HumanFaceDetect *detect = new HumanFaceDetect();

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


esp_err_t camera_init(){
    ESP_LOGE(TAG, "Camera Init!");

    //esp32-camera ライブラリのコンフィグ。
    //camera_config_t の中身を色々いじると、XIAO とか他のボードやカメラでも使えるようになる
    //ov2640 は JPEG / RGB565 / RGB888 対応だったり、
    //GC0308 (M5Stack Core S3 カメラ)は JPEG 非対応だったり、機種によって違う
    camera_config_t camera_config_for_coreS3 = BSP_CAMERA_DEFAULT_CONFIG;
    //camera_config.xclk_freq_hz = 20000000;
    camera_config_for_coreS3.pixel_format = PIXFORMAT_RGB565;
    camera_config_for_coreS3.frame_size = FRAMESIZE_QVGA;
    camera_config_for_coreS3.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    

    esp_err_t err = esp_camera_init(&camera_config_for_coreS3);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera Init Failed");
        return ESP_FAIL;
    }

    return ESP_OK;
}


esp_err_t camera_capture_and_infer(){
    //ESP_LOGE(TAG, "Camera Capture Start!");
    camera_fb_t * fb = esp_camera_fb_get();             //カメラ画像取得。先のカメラ設定で RGB565 / JPEG とかの形式で取得される
    if (!fb) {
        ESP_LOGE(TAG, "Camera Capture Failed");
        M5.Display.println("Camera Capture Failed");
        return ESP_FAIL;
    }
    //ESP_LOGE(TAG, "Camera Capture End!");


    //[Camera(with RGB565 setting)] -> [dl::image::img_t]
    //カメラ画像がRGB565 の場合、そのRGB565 を esp-dl の形式へ。
    dl::image::img_t newdata;
    newdata.data = fb->buf;
    newdata.height = fb->height;
    newdata.width = fb->width;
    newdata.pix_type = dl::image::DL_IMAGE_PIX_TYPE_RGB565;


    
    //推論モデルにより入力画像形式が色々です。
    //このソースのサンプルで利用してる HumanFaceDetect は RGB565なのでカメラ画像を直接入れてる感じ。
    //PINTO さんの UHD モデルは設定により RGB888 / YUV422 などなので変換入力が必要。

    //カメラ画像がJPEG の場合、そのJPEG を esp-dl のJPEG形式へ。
    //   [Camera(with JPEG setting)] -> [dl::image::jpeg_img_t]
    //dl::image::jpeg_img_t newjpgdata;
    //newjpgdata.data = fb->buf;
    //newjpgdata.data_len = fb->len;

    //esp-dl のJPEG形式を RGB565 / RGB888 などの形式へ。
    //   [dl::image::jpeg_img_t] to [dl::image_img_t (RGB888 or RGB565) with some endian pattern]
    //   #include "dl_image_jpeg.hpp" が必要
    //auto img = dl::image::sw_decode_jpeg(newjpgdata, dl::image::DL_IMAGE_PIX_TYPE_RGB888);
    //auto img2 = dl::image::sw_decode_jpeg(newjpgdata, dl::image::DL_IMAGE_PIX_TYPE_RGB565, dl::image::DL_IMAGE_CAP_RGB565_BIG_ENDIAN);


    canvas.writePixels((uint16_t*)fb->buf, int(fb->len/2));         //M5Canvas 内に画像書き込み。



    //推論部
    //esp-dl のモデルは、run() したら結果リストが来る便利設計！
    auto &detect_results = detect->run(newdata);
    for (const auto &res : detect_results) {
        ESP_LOGI(TAG,
                 "[score: %f, x1: %d, y1: %d, x2: %d, y2: %d]",
                 res.score,
                 res.box[0],
                 res.box[1],
                 res.box[2],
                 res.box[3]);
        ESP_LOGI(
            TAG,
            "left_eye: [%d, %d], left_mouth: [%d, %d], nose: [%d, %d], right_eye: [%d, %d], right_mouth: [%d, %d]]",
            res.keypoint[0],
            res.keypoint[1],
            res.keypoint[2],
            res.keypoint[3],
            res.keypoint[4],
            res.keypoint[5],
            res.keypoint[6],
            res.keypoint[7],
            res.keypoint[8],
            res.keypoint[9]);


            //バウンディングボックス位置描画
            int wid = (res.box[2]-res.box[0]);
            int hei = (res.box[3]-res.box[1]);
            canvas.drawRect(res.box[0], res.box[1], wid, hei, TFT_RED);
            for (int a=0; a<=9; a+=2) {
              canvas.fillCircle(res.keypoint[a], res.keypoint[a+1], 8, TFT_GREEN);      //M5Canvas 内にバウンディングボックス書き込み。
            }
    }

    canvas.pushSprite(0, 0);                //M5Canvas 描画更新。


    //カメラフレームバッファ開放
    esp_camera_fb_return(fb);
    return ESP_OK;
}




extern "C" void app_main(void)
{
    //M5Unified 初期化
    auto cfg = M5.config();
    cfg.output_power = true;
    M5.begin(cfg);


    //M5Unified 動作確認
    M5.Display.setFont(&fonts::efontJA_24);
    M5.Display.println("HelloWorld");
    ESP_LOGE(TAG, "HelloWorld");


    canvas.createSprite(320, 240);                  //M5Canvas 初期化（M5.Display だけだとチラつくので M5Canvas 利用する）


    //SD カードのマウント（menuconfig で推論モデルをSDから利用する設定の時）
    #if CONFIG_HUMAN_FACE_DETECT_MODEL_IN_SDCARD
        ESP_ERROR_CHECK(bsp_sdcard_mount());
    #endif


    // カメラ初期化
    camera_init();
    

    while(1) {
        //カメラ画像から推論
        camera_capture_and_infer();
    }
    
    canvas.deleteSprite();              //M5Canvas 消去

    delete detect;                      //推論クラス delete


    //SD カードのアンマウント（menuconfig で推論モデルをSDから利用する設定の時）
    #if CONFIG_HUMAN_FACE_DETECT_MODEL_IN_SDCARD
        ESP_ERROR_CHECK(bsp_sdcard_unmount());
    #endif

}