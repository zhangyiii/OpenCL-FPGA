#include "cnn.hpp"

using namespace cnn;

#define NUM_TEST 10

namespace test {

    // Run time test with single input.
    void runTimeTest(std::ofstream &o, CNN *cnn, const vec &in) {

        writeXMLOpenTag(o, "single");

        unsigned long long totalTime = 0;
        for (size_t i = 0; i < NUM_TEST; ++i) {
            totalTime += cnn->forwardCL(in);
        }
        size_t averageTime = totalTime / NUM_TEST;
        std::cout << "Average Time: " << averageTime << std::endl;
        writeXMLTag(o, "averageTime", averageTime);
        writeXMLCloseTag(o, "single");
        std::cout << "Finish testing!" << std::endl;
    }

    // Run time test with batch input.
    void runTimeTestBatch(std::ofstream &o, CNN *cnn, const vec &in, size_t n) {
        vec out;
        cl_int err;
        cl_ulong t;
        writeXMLOpenTag(o, "batch");
        std::vector<cl_event> events = cnn->forwardCLBatch(in, out, n);
        size_t eventForOneInput = events.size() / n;

        for (size_t i = 0; i < n; ++i) {
            writeXMLOpenTag(o, "input");
            for (size_t e = 0; e < eventForOneInput; ++e) {
                writeXMLOpenTag(o, "event");

                err = clGetEventProfilingInfo(events[i * eventForOneInput + e], CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &t, NULL);
                handleError(err, "Failed get profile. ");
                writeXMLTag(o, "que", t);

                err = clGetEventProfilingInfo(events[i * eventForOneInput + e], CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &t, NULL);
                handleError(err, "Failed get profile. ");
                writeXMLTag(o, "sub", t);

                err = clGetEventProfilingInfo(events[i * eventForOneInput + e], CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &t, NULL);
                handleError(err, "Failed get profile. ");
                writeXMLTag(o, "sta", t);

                err = clGetEventProfilingInfo(events[i * eventForOneInput + e], CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &t, NULL);
                handleError(err, "Failed get profile. ");
                writeXMLTag(o, "end", t);

                writeXMLCloseTag(o, "event");
            }
            writeXMLCloseTag(o, "input");
        }

        writeXMLCloseTag(o, "batch");
        std::cout << "Finish testing!" << std::endl;
    }

    void runFuncTest(CNN *cnn, const vec &in) {
        cnn->forwardCL(in);
        cnn::vec outCL(cnn->getOut());
        cnn->forwardCPU(in);
        cnn::vec outCPU(cnn->getOut());
        for (int i = 0; i < outCL.size(); ++i) {
            if (abs(outCL[i] - outCPU[i]) > 0.0001f) {
                std::cerr << "Failed test: CL kernel works incorrect. " << std::endl;
                exit(-2);
            }
        }
        std::cout << "CL Kernel works perfect!. " << std::endl;
    }
}