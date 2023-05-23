#pragma once

#include "face_api_test_I.h"
#include "utils_I.h"

using namespace std;
using namespace FACEAPITEST;

void FACEAPI_extract(shared_ptr<IdentInterface> face_api_ptr, params_type& params, const string& output_dir);

ReturnStatus createTemplateParam(shared_ptr<IdentInterface> face_api_ptr, const Multiface &faces, TemplateRole role, vector<uint8_t> &templ, vector<EyePair> &eyeCoordinates, vector<double>&);

class FACEAPI
{
public:
    static void search(shared_ptr<IdentInterface> face_api_ptr, params_type& params, const string& output_dir);
    static void tpir(const string& output_dir);
    static void insert(shared_ptr<IdentInterface> face_api_ptr, params_type& params, const string& output_dir);
    static void remove(shared_ptr<IdentInterface> face_api_ptr, params_type& params);

private:
    constexpr static int mc_ranks[] = {1, 5, 20};
};
