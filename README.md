# M5Stack Core S3 - Face Detection

<br>Core S3 で顔認識するベースとなる最低限なコードを置いておきます。
<br>
<br>esp-dl という Espressif 公式の機械学習モデルの実行ライブラリを使用しているので、他のモデル利用にも改変して使えると思います。
<br>ESP-IDF に慣れない方もいるかと思いますが（自分含め）、とりあえずこのプロジェクトのフォルダを ESP-IDF で開いてビルドして書き込めばそれで使えるようになっています！
<br>
<br>




https://github.com/user-attachments/assets/bc08ac50-c18f-451c-b7b9-f42a6844af3e






# 環境

<br>M5Stack Core S3 (カメラつき GC0308) 
<br>ESP-IDF (v5.5.2, windows)
<br>M5Unified (0.2.10)

<br>私は ESP-IDF (win) インストーラでインストールしてから、VS Code の extension 入れて、インストールパスを指定して利用しました。（PlatformIO も併用できる環境になった）
<br>
<br>


# 手順

（ESP-IDF インストールや操作などは他サイトや AI 参照）

<br>①ESP-IDF インストール
<br>②(idf.py menuconfig で必要な設定は済ませてありますのでスキップでOK)
<br>③ビルド
<br>④書き込み
<br>
<br>


# 他のモデルの利用について

たとえば @PINTO03091 さんが ESP32 用モデル (espdl 形式)でご自身の高速モデルを変換して公開されています。

https://github.com/PINTO0309/esp-who/tree/custom/examples/ultra_lightweight_human_detection

https://github.com/PINTO0309/UHD

<br>ultra_lightweight_human_detection/components/uhd_detect ディレクトリは他の Espressif esp-dl 公式 examples のモデルと同様に run() で結果が返る形式のコンポーネントになっています。
<br>なので、画像を指定された形で（RGB888 や YUV422 など）入力すれば、マイコンで推論が実行されます。便利！
<br>esp-dl/examples, esp-dl/models 内を参考にしてコード少しいじると、他モデルが利用できます。
<br>esp-ppq というツールで、自分で onnx モデルを *.espdl に量子化・変換することができるようです！うおおお！
<br>（自分が試したときの、RGB888 変換などの関数利用例も一部コードに残しておきました）
<br>
<br>




# esp-dl examples 各モデルの速度やファイルサイズ

<img width="1671" height="937" alt="image" src="https://github.com/user-attachments/assets/5181ddbc-91cc-4e09-a6ea-a7b60d1bae62" />
<br>
<br>

# 関連 examples

## ● esp-dl

https://github.com/espressif/esp-dl/tree/master/examples

https://github.com/espressif/esp-dl/tree/master/models

各種学習モデルが、コード上で run() すれば結果が返るような形のライブラリになっています。便利！
<br>
<br>

## ● esp-who

https://github.com/espressif/esp-who/tree/master/examples

本来は公式で esp-dl 利用したコードがあるのですが、ちょっと ESP-IDF 初心者の自分にはわかりづらかったので、今回のようなシンプルコードにしてみました。
<br>
<br>

## ● esp-idf 公式 examples

https://github.com/espressif/esp-idf/tree/master/examples

基本の UART や I2C, ほかさまざまな ESP-IDF の使い方がのっています。
<br>
<br>

## ● ESP32 各種ボードのサポートパッケージ

https://github.com/espressif/esp-bsp/tree/master/examples

Core 2 や M5 Dial などなど、各種初期化がある程度簡単に設定済みのライブラリです。自分のこのリポジトリでも Core S3 用を利用してカメラピン設定などが省略できています。
<br>もしここにない XIAO ESP32S3 など他のボードで利用する場合も、カメラピンなど番号変えたりすることで利用できます。
<br>


# LICENSE

MIT LICENSE
<br>@devemin


