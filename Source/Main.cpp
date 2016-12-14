//--------//--------//--------//--------//--------//--------//--------//--------
/*
    The MIT License (MIT)

    Copyright (c) 2016 John Flynn

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.

//--------//--------//--------//--------//--------//--------//--------//--------

    Filter Classification.

    Takes raw coefficients of a 2-point FIR filter and classifies it as...
    
    0: Close to a low pass filter
    1: Close to a high pass filter
    2: Pretty much does nothing!

*/

#include "../JuceLibraryCode/JuceHeader.h"

//--------//--------//--------//--------//--------//--------//--------//--------

String filterTypeToString (int filterType)      // human readable output!
{
    switch (filterType) {
    case 0:
        return "Closer to a low pass filter  ";
    case 1:
        return "Closer to a high pass filter ";
    case 2:
        return "Pretty much the same         ";
    default:
        return "Neither high or low, error!";
    }
}

//--------//--------//--------//--------//--------//--------//--------//--------

int main(int argc, const char* argv[])
{
    juce::UnitTestRunner runner;
    runner.runAllTests();

    return 0;
}

//--------//--------//--------//--------//--------//--------//--------//--------

class ClassificationTest  : public UnitTest
{
public:
    ClassificationTest();
    void runTest() override;
};
static ClassificationTest ClassificationTest;

ClassificationTest::ClassificationTest() : UnitTest ("Classify filters") {}

void ClassificationTest::runTest()
{
    enum FilterType                                 // create dataset
    {
        isLoPass,
        isHiPass,
        noFilter
    };
    
    Array<Classification::DataSample> trainingSet;

    //==============================================================================

    Classification::DataSample loPass;              // 1. first attempt, just 3 data samples
    Classification::DataSample noFilt;
    Classification::DataSample hiPass;
    loPass.inputs  = {1.0,  1.0};
    noFilt.inputs  = {1.0,  0.0};
    hiPass.inputs  = {1.0, -1.0};
    loPass.outputs  = {FilterType::isLoPass};
    noFilt.outputs  = {FilterType::noFilter};
    hiPass.outputs  = {FilterType::isHiPass};
    trainingSet.add (loPass);
    trainingSet.add (noFilt);
    trainingSet.add (hiPass);

    Classification::DataSample loPass2;             // 2. next attempt. if bass/treble difference
    Classification::DataSample hiPass2;             // change is more than ~2dB, classify as filter
    loPass2.inputs = {1.0,  0.1};                   // otherwise say it's pass audio
    hiPass2.inputs = {1.0, -0.1};
    loPass2.outputs = {FilterType::isLoPass};
    hiPass2.outputs = {FilterType::isHiPass};
    trainingSet.add (loPass2);
    trainingSet.add (hiPass2);

    Classification::DataSample loPass3;             // 3. make sure inverted gives correct
    Classification::DataSample hiPass3;             // results
    loPass3.inputs = {-1.0, -1.0};
    hiPass3.inputs = {-1.0,  1.0};
    loPass3.outputs = {FilterType::isLoPass};
    hiPass3.outputs = {FilterType::isHiPass};
    trainingSet.add (loPass3);
    trainingSet.add (hiPass3);

    //==============================================================================

    Classification classification;                  // train it
    classification.initialize();
    classification.train (trainingSet);

    Array<double> data;                             // process some data
    Array<int> out;

    //==============================================================================

    beginTest ("Training examples 1");

    data = {1.0, 1.0};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isLoPass));

    data = {1.0, -1.0};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isHiPass));

    data = {1.0, 0.0};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::noFilter));

    //==============================================================================

    beginTest ("Training examples 2");

    data = {1.0, 0.25};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isLoPass));

    data = {1.0, -0.25};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isHiPass));

    //==============================================================================

    beginTest ("Threshold between lowpass and 'no filter'");

    data = {1.0, 0.1};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isLoPass));

    data = {1.0, 0.075};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isLoPass));

    data = {1.0, 0.05};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::noFilter));

    //==============================================================================

    beginTest ("Threshold between hipass and 'no filter'");

    data = {1.0, -0.1};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isHiPass));

    data = {1.0, -0.075};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isHiPass));

    data = {1.0, -0.05};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::noFilter));

    //==============================================================================

    beginTest ("Inverted values should be correct");

    data = {-1.0, -1.0};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isLoPass));

    data = {-1.0,  1.0};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isHiPass));

    data = {-1.0, -0.25};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isLoPass));

    data = {-1.0,  0.25};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isHiPass));

    //==============================================================================

    beginTest ("Try random values");

    Random rnd = getRandom();
    rnd.setSeed(0);

    data = {rnd.nextDouble() * 2 - 1, rnd.nextDouble() * 2 - 1};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isLoPass));

    data = {rnd.nextDouble() * 2 - 1, rnd.nextDouble() * 2 - 1};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isLoPass));

    data = {rnd.nextDouble() * 2 - 1, rnd.nextDouble() * 2 - 1};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isLoPass));

    data = {rnd.nextDouble() * 2 - 1, rnd.nextDouble() * 2 - 1};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isLoPass));

    data = {rnd.nextDouble() * 2 - 1, rnd.nextDouble() * 2 - 1};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isHiPass));

    data = {rnd.nextDouble() * 2 - 1, rnd.nextDouble() * 2 - 1};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isHiPass));

    data = {rnd.nextDouble() * 2 - 1, rnd.nextDouble() * 2 - 1};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isLoPass));

    data = {rnd.nextDouble() * 2 - 1, rnd.nextDouble() * 2 - 1};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isHiPass));

    data = {rnd.nextDouble() * 2 - 1, rnd.nextDouble() * 2 - 1};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isHiPass));

    data = {rnd.nextDouble() * 2 - 1, rnd.nextDouble() * 2 - 1};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isLoPass));

    data = {rnd.nextDouble() * 2 - 1, rnd.nextDouble() * 2 - 1};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::isLoPass));

    data = {rnd.nextDouble() * 2 - 1, rnd.nextDouble() * 2 - 1};
    out = classification.process(data);
    std::cout << filterTypeToString(out[0]) << ": {" << data[0] << ", " << data[1] << "}\n";
    expectEquals (out[0], static_cast<int> (FilterType::noFilter));

    //==============================================================================
}

