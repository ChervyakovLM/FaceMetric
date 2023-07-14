# FaceMetric

REQUIREMENTS\
glog\
tclap\
opencv>=4.6.0

BUILD\
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../Release -DTCLAP_INCLUDE_DIR={PATH}/tclap/include -DFACE_API_ROOT_DIR={PATH}/FaceMetric/CI/face_api_test ..

RUN VERIFICATION

FLAGS\
--split - path to split directory, required\
--config - path to FaceEngine config directory, default: input/config\
--extract_list - path to extract list file, default: input/extract.txt\
--extract_prefix - path to images directory, default: input/images\
--grayscale - open images as grayscale, default: false\
--count_proc - count extract processes, default: thread::hardware_concurrency()\
--extra_timings - print extra timings: percentile, min, max, std_dev, default: false\
--extract_info - logging additional extract results: eyes, quality, etc, default: false\
--debug_info - logging debug output, default: false\
--desc_size - descriptor size, default: 512\
--percentile - percentile in %, default: 90\
--do_extract - do extract stage, default: true\
--do_match - do match stage, default: true\
--do_ROC - do calc ROC stage, default: true\

./checkFaceApi_V --split=./verification

OUT\
fpr - 10^-4, tpr - 0.828\
fpr - 10^-5, tpr - 0.740\
fpr - 10^-6, tpr - 0.628\
fpr - 10^-7, tpr - 0.460\
fpr - 10^-8, tpr - 0.133\

RUN IDENTIFICATION 

FLAGS\
--split - path to split directory, required\
--config - path to FaceEngine config directory, default: input/config\
--db_list - path to db list file, default: input/db.txt\
--mate_list - path to mate queries list file, default: input/mate.txt\
--nonmate_list - path to nonmate queries list file, default: input/nonmate.txt\
--insert_list - path to insert list file, default: input/insert.txt\
--remove_list - path to remove list file, default: input/remove.txt\
--extract_prefix - path to images directory, default: input/images\
--grayscale - open images as grayscale, default: false\
--count_proc - count extract processes, default: thread::hardware_concurrency()\
--extra_timings - print extra timings: percentile, min, max, std_dev, default: false\
--extract_info - logging additional extract results: eyes, quality, etc, default: false\
--debug_info - logging debug output, default: false\
--desc_size - descriptor size, default: 512\
--percentile - percentile in %, default: 90\
--nearest_count - nearest count, false, 100\
--search_info - logging additional search results: decision, default: false\
--do_extract - do extract stage, default: true\
--do_graph - do create graph stage, default: true\
--do_insert - do insert stage, default: true\
--do_remove - do remove stage, default: true\
--do_search - do search stage, default: true\
--do_tpir - do calc TPIR/FPIR stage, default: true\

./checkFaceApi_I --split=./identification

OUT\
I0824 17:56:28.947021 54252 in_out.cpp:224] General:\
fpir - 10^-1, tpir - 0.942\
fpir - 10^-2, tpir - 0.887\
fpir - 10^-3, tpir - 0.408\

I0824 17:56:28.947422 54252 in_out.cpp:224] Rank 1:\
fpir - 10^-1, tpir - 0.940\
fpir - 10^-2, tpir - 0.887\
fpir - 10^-3, tpir - 0.408\

I0824 17:56:28.947809 54252 in_out.cpp:224] Rank 5:\
fpir - 10^-1, tpir - 0.942\
fpir - 10^-2, tpir - 0.887\
fpir - 10^-3, tpir - 0.408\

I0824 17:56:28.948201 54252 in_out.cpp:224] Rank 20:\
fpir - 10^-1, tpir - 0.942\
fpir - 10^-2, tpir - 0.887\
fpir - 10^-3, tpir - 0.408\
