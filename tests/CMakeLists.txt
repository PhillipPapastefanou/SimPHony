set(headers

        )

set(source
        general/water_flow_segments_approx_test.cpp
        general/soil_models_test.cpp
        general/special_functions_test.cpp

        hainich/test_driver_exist.cpp
        hainich/source_hainich_direct_test.cpp
        hainich/source_hainich_from_file_test.cpp
        hainich/source_hainich_multi_from_file_test.cpp

        swiss_cc/source_swiss_cc_direct_test.cpp
        swiss_cc/source_swiss_cc_direct_from_file_test.cpp
        swiss_cc/source_swiss_cc_single_test.cpp
        swiss_cc/source_swiss_cc_multi_test.cpp

        main.cpp
        )
include(add_test_sources)