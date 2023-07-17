**A framework for testing the speed and accuracy of various neural networks in the tasks of face recognition (verification and identification)**

The task of face recognition is one of the most in-demand computer vision tasks at the moment. The task is divided into 2 main subtasks. Verification, comparison of biometric templates 1:1 (one to one). Identification, comparison of biometric templates 1:N (one to many). One of the challenges facing the industry is the difficulty of comparing different machine learning algorithms with each other, both in terms of accuracy and speed. The framework provides a unified functionality for testing the accuracy and speed metrics of any external libraries and frameworks that comply with the framework API specification, or have an adapter to unify the API.

The practical significance of the problem being solved lies in the possibility of a unified and uniform comparison of the accuracy and speed metrics of various libraries with each other and obtaining an independent assessment of these metrics. It can be used as a measuring tool for various contests and competitions in the field of face recognition. An example, development teams or companies, it doesn't matter, submit their face recognition libraries to some kind of competition, and they need to be compared somehow. but everyone has a different API. Contest participants write an API adapter for mutual integration and the framework created within the framework of the project compares them independently, who is more accurate in one or another functionality (verification, identification)

This tool is a software application for evaluating face recognition software metrics. List of directions of applied use:

– assessment of metrics of speed and accuracy of face recognition libraries in case of acquisition / purchase of third-party software for independent testing

– assessment of metrics of speed and accuracy of face recognition libraries in case of choosing several solutions for use for independent and unified assessment.

– facial recognition competitions among teams and companies specializing in this area

– assessment of the metrics of speed and accuracy of face recognition libraries under given operating conditions, such as, for example, searching for faces in the database after modifying the search index, and the like.

List of functions measured by the software for application use:

– Extraction of biometric templates for the task of verifying faces

– Matching biometric templates for the task of verifying faces

– Extraction of biometric templates for face identification task

– Building a search index for the task of identifying persons

– Search for biometric templates in the index for the task of identifying faces

– Inserting new biometric templates into the index for the task of identifying faces

– Removal of given biometric templates from the index for the task of identifying persons

**Project philosophy**

- Simple to get value
- Flexible and easy to add new own "AI steps".
- Incrementally build towards a user experience of:
  1. high level prompting
  2. giving feedback to the AI that it will remember over time
- Fast handovers back and forth between AI and human
- Simplicity, all computation is "resumable" and persisted to the filesystem

**Features**

The framework has the following technical characteristics:

- Maximum biometric template size supported: 8 KB.

- Maximum search index size: 1 million patterns.

- Maximum template extraction time: 2 min.

- Maximum supported image resolution: 2560 x 2048 pixels.

- measurement accuracy: 10 μs.

- build system: cmake or qmake.

- Ability to configure the stages of working with templates.

- Ability to skip one of the stages of working with templates.

- Supported image formats: BMP, JPEG, PNG.

- Logging lengthy operations to a file.

- Ability to save received templates to the hard drive.

- Possibility of parallel calculation of templates on different processor cores at the same time.

- Retrievable characteristics of the execution time of operations: average time, minimum time, maximum time, standard deviation.

- Increasing the speed of testing new libraries and frameworks for face recognition.

- Unification of the interface, data and metrics.

- Acceleration of calculations through the use of parallel processing is proportional to the number of physical cores of the server.

- Improving the fault tolerance of the metrics calculation algorithm by saving data at key stages.

- Foreign open-source libraries used in the development must not contain malicious components.

- Development language: C++.

- OS: linux.

**How does it work?**

REQUIREMENTS\
 glog\
 tclap\
 opencv\>=4.6.0

BUILD\
 cmake -DCMAKE\_BUILD\_TYPE=Release -DCMAKE\_INSTALL\_PREFIX=../Release -DTCLAP\_INCLUDE\_DIR={PATH}/tclap/include -DFACE\_API\_ROOT\_DIR={PATH}/FaceMetric/CI/face\_api\_test ..

RUN VERIFICATION\
Performing verification steps:
 - extraction of biometric templates
 - calculation of the degree of correspondence between biometric templates
 - calculation of TPR/FPR metrics
   
Running all stages:\
 ./checkFaceApi_V –split=./verification
 
Starting the biometric template extraction step:\
 ./checkFaceApi_V –split=./verification –do_match=0 –do_ROC=0
 
Starting the stage of calculating the degree of correspondence between biometric templates:\
 ./checkFaceApi_V –split=./verification –do_extract=0 –do_ROC=0
 
Starting the TPR/FPR metrics calculation step:\
 ./checkFaceApi_V –split=./verification –do_extract=0 –do_match=0

FLAGS\
 --split - path to split directory, required\
 --config - path to FaceEngine config directory, default: input/config\
 --extract\_list - path to extract list file, default: input/extract.txt\
 --extract\_prefix - path to images directory, default: input/images\
 --grayscale - open images as grayscale, default: false\
 --count\_proc - count extract processes, default: thread::hardware\_concurrency()\
 --extra\_timings - print extra timings: percentile, min, max, std\_dev, default: false\
 --extract\_info - logging additional extract results: eyes, quality, etc, default: false\
 --debug\_info - logging debug output, default: false\
 --desc\_size - descriptor size, default: 512\
 --percentile - percentile in %, default: 90\
 --do\_extract - do extract stage, default: true\
 --do\_match - do match stage, default: true\
 --do\_ROC - do calc ROC stage, default: true

OUTPUT EXAMPLE\
 fpr - 10^-4, tpr - 0.828\
 fpr - 10^-5, tpr - 0.740\
 fpr - 10^-6, tpr - 0.628\
 fpr - 10^-7, tpr - 0.460\
 fpr - 10^-8, tpr - 0.133

RUN IDENTIFICATION\
Performing identification steps:
 - extraction of biometric templates
 - building a search index
 - search for close biometric templates
 - inserting biometric templates into the search index
 - removal of biometric templates from the search index
 - calculation of TPIR/FPIR metrics

Running all stages:\
 ./checkFaceApi_I –split=./identification
 
Starting the biometric template extraction step:\
 ./checkFaceApi_I –split=./identification –do_graph=0 –do_search=0 –do_insert=0 –do_remove=0 –do_tpir=0
 
Starting the stage of building a search index:\
 ./checkFaceApi_I –split=./identification –do_extract=0 –do_search=0 –do_insert=0 –do_remove=0 –do_tpir=0

Launching the stage of searching for similar biometric templates:\
 ./checkFaceApi_I –split=./identification –do_extract=0 –do_graph=0 –do_insert=0 –do_remove=0 –do_tpir=0

Launching the stage of inserting biometric templates into the search index:\
 ./checkFaceApi_I –split=./identification –do_extract=0 –do_graph=0 –do_search=0 –do_remove=0 –do_tpir=0
 
Starting the step of removing biometric templates from the search index:\
 ./checkFaceApi_I –split=./identification –do_extract=0 –do_graph=0 –do_search=0 –do_insert=0 –do_tpir=0
 
Starting the TPIR/FPIR metrics calculation step:\
 ./checkFaceApi_I –split=./identification –do_extract=0 –do_graph=0 –do_search=0 –do_insert=0 –do_remove=0

FLAGS\
 --split - path to split directory, required\
 --config - path to FaceEngine config directory, default: input/config\
 --db\_list - path to db list file, default: input/db.txt\
 --mate\_list - path to mate queries list file, default: input/mate.txt\
 --nonmate\_list - path to nonmate queries list file, default: input/nonmate.txt\
 --insert\_list - path to insert list file, default: input/insert.txt\
 --remove\_list - path to remove list file, default: input/remove.txt\
 --extract\_prefix - path to images directory, default: input/images\
 --grayscale - open images as grayscale, default: false\
 --count\_proc - count extract processes, default: thread::hardware\_concurrency()\
 --extra\_timings - print extra timings: percentile, min, max, std\_dev, default: false\
 --extract\_info - logging additional extract results: eyes, quality, etc, default: false\
 --debug\_info - logging debug output, default: false\
 --desc\_size - descriptor size, default: 512\
 --percentile - percentile in %, default: 90\
 --nearest\_count - nearest count, false, 100\
 --search\_info - logging additional search results: decision, default: false\
 --do\_extract - do extract stage, default: true\
 --do\_graph - do create graph stage, default: true\
 --do\_insert - do insert stage, default: true\
 --do\_remove - do remove stage, default: true\
 --do\_search - do search stage, default: true\
 --do\_tpir - do calc TPIR/FPIR stage, default: true

./checkFaceApi\_I --split=./identification

OUTPUT EXAMPLE\
I0824 17:56:28.947021 54252 in\_out.cpp:224] General:\
 fpir - 10^-1, tpir - 0.942\
 fpir - 10^-2, tpir - 0.887\
 fpir - 10^-3, tpir - 0.408

I0824 17:56:28.947422 54252 in\_out.cpp:224] Rank 1:\
 fpir - 10^-1, tpir - 0.940\
 fpir - 10^-2, tpir - 0.887\
 fpir - 10^-3, tpir - 0.408

I0824 17:56:28.947809 54252 in\_out.cpp:224] Rank 5:
 fpir - 10^-1, tpir - 0.942\
 fpir - 10^-2, tpir - 0.887\
 fpir - 10^-3, tpir - 0.408

I0824 17:56:28.948201 54252 in\_out.cpp:224] Rank 20:\
 fpir - 10^-1, tpir - 0.942\
 fpir - 10^-2, tpir - 0.887\
 fpir - 10^-3, tpir - 0.408

**License**

This project is [MIT](https://github.com/your_username/your_repository/blob/master/LICENSE) licensed.
