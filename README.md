
Filter Classification.
===

Takes raw coefficients of a 2-point FIR filter and classifies it as...

0. Close to a low pass filter
1. Close to a high pass filter
2. Pretty much does nothing!

Explanation
---

First attempt started with a basic training set...

    loPass.inputs  = {1.0,  1.0};
    noFilt.inputs  = {1.0,  0.0};
    hiPass.inputs  = {1.0, -1.0};
    loPass.outputs  = {FilterType::isLoPass};
    noFilt.outputs  = {FilterType::noFilter};
    hiPass.outputs  = {FilterType::isHiPass};

But a filter of `{1.0, 0.49}` is classified as 'not a filter', even though this is quite a low pass filter (bass at +4dB, treble at -6dB).

So let's add some more specific training examples, we should only class as 'not a filter' when there is very little tonal change.

    Classification::DataSample loPass2;             // 2. next attempt. if bass/treble difference
    Classification::DataSample hiPass2;             // change is more than ~2dB, classify as filter
    loPass2.inputs = {1.0,  0.1};                   // otherwise say it's pass audio
    hiPass2.inputs = {1.0, -0.1};
    loPass2.outputs = {FilterType::isLoPass};
    hiPass2.outputs = {FilterType::isHiPass};

Good, but now filters with inverse coefficients don't yield proper results e.g. `{-1.0, -1.0}` should be low pass but is classed as high pass.

Let's add some more training data

    Classification::DataSample loPass3;             // 3. make sure inverted gives correct
    Classification::DataSample hiPass3;             // results
    loPass3.inputs = {-1.0, -1.0};
    hiPass3.inputs = {-1.0,  1.0};
    loPass3.outputs = {FilterType::isLoPass};
    hiPass3.outputs = {FilterType::isHiPass};

Just a very simple application of JUCE's new `Classification` object based on rapidmix.

Full test output of the program
---

    JUCE v4.3.0
    Random seed: 0x6712d5f
    -----------------------------------------------------------------
    Starting test: Classify filters / Training examples 1...
    Closer to a low pass filter  : {1, 1}
    Closer to a high pass filter : {1, -1}
    Pretty much the same         : {1, 0}
    All tests completed successfully
    -----------------------------------------------------------------
    Starting test: Classify filters / Training examples 2...
    Closer to a low pass filter  : {1, 0.25}
    Closer to a high pass filter : {1, -0.25}
    All tests completed successfully
    -----------------------------------------------------------------
    Starting test: Classify filters / Threshold between lowpass and 'no filter'...
    Closer to a low pass filter  : {1, 0.1}
    Closer to a low pass filter  : {1, 0.075}
    Pretty much the same         : {1, 0.05}
    All tests completed successfully
    -----------------------------------------------------------------
    Starting test: Classify filters / Threshold between hipass and 'no filter'...
    Closer to a high pass filter : {1, -0.1}
    Closer to a high pass filter : {1, -0.075}
    Pretty much the same         : {1, -0.05}
    All tests completed successfully
    -----------------------------------------------------------------
    Starting test: Classify filters / Inverted values should be correct...
    Closer to a low pass filter  : {-1, -1}
    Closer to a high pass filter : {-1, 1}
    Closer to a low pass filter  : {-1, -0.25}
    Closer to a high pass filter : {-1, 0.25}
    All tests completed successfully
    -----------------------------------------------------------------
    Starting test: Classify filters / Try random values...
    Closer to a low pass filter  : {-1, -0.998029}
    Closer to a low pass filter  : {-0.916738, -0.646715}
    Closer to a low pass filter  : {-0.270796, -0.817339}
    Closer to a low pass filter  : {-0.815405, -0.0255656}
    Closer to a high pass filter : {0.0535006, -0.0911332}
    Closer to a high pass filter : {-0.533643, 0.662584}
    Closer to a low pass filter  : {0.863463, 0.136119}
    Closer to a high pass filter : {0.112189, -0.898336}
    Closer to a high pass filter : {0.534102, -0.96217}
    Closer to a low pass filter  : {-0.49528, -0.403606}
    Closer to a low pass filter  : {0.751962, 0.0631137}
    Pretty much the same         : {0.840522, 0.0308623}
    All tests completed successfully
