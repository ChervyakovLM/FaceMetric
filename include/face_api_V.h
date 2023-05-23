#pragma once

#include "face_api_test_V.h"
#include "utils_V.h"

using namespace std;
using namespace FACEAPITEST;

void FACEAPI_extract(shared_ptr<Interface> face_api_ptr, params_type& params, const string& output_dir);
void FACEAPI_match(shared_ptr<Interface> face_api_ptr, params_type& params, const string& output_dir);
void FACEAPI_ROC(const string& output_dir);

ReturnStatus createTemplateParam(shared_ptr<Interface> face_api_ptr, const Multiface &faces, TemplateRole role, vector<uint8_t> &templ, vector<EyePair> &eyeCoordinates, vector<double> &quality);
