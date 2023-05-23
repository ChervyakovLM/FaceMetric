# FaceMetric

opencv >=4.5.4 (build from src)
https://docs.opencv.org/4.x/d0/dd4/tutorial_dnn_face.html?msclkid=bbba05a1af3911eca0d1cf4ec0faac6c

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../Release -DTCLAP_INCLUDE_DIR=./nist/CI/tclap/include -DFACE_API_ROOT_DIR=./CI/face_api_test -DFREEIMAGE_ROOT_DIR=./CI/FreeImage ..

run verification
./checkFaceApi_V --split=./split

fpr - 10^-4, tpr - 0.828
fpr - 10^-5, tpr - 0.740
fpr - 10^-6, tpr - 0.628
fpr - 10^-7, tpr - 0.460
fpr - 10^-8, tpr - 0.133

run identification 
./checkFaceApi_I --split=./split2

I0824 17:56:28.947021 54252 in_out.cpp:224] General:
fpir - 10^-1, tpir - 0.942
fpir - 10^-2, tpir - 0.887
fpir - 10^-3, tpir - 0.408

I0824 17:56:28.947422 54252 in_out.cpp:224] Rank 1:
fpir - 10^-1, tpir - 0.940
fpir - 10^-2, tpir - 0.887
fpir - 10^-3, tpir - 0.408

I0824 17:56:28.947809 54252 in_out.cpp:224] Rank 5:
fpir - 10^-1, tpir - 0.942
fpir - 10^-2, tpir - 0.887
fpir - 10^-3, tpir - 0.408

I0824 17:56:28.948201 54252 in_out.cpp:224] Rank 20:
fpir - 10^-1, tpir - 0.942
fpir - 10^-2, tpir - 0.887
fpir - 10^-3, tpir - 0.408
