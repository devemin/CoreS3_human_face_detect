set(src_dirs        ./)
set(include_dirs    ./)

set(requires        human_face_detect
                    m5stack_core_s3_noglib
                    )

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires} )