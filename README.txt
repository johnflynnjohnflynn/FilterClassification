
Filter Classification.
===

Takes raw coefficients of a 2-point FIR filter and classifies it as...
    
0. Close to a low pass filter
1. Close to a high pass filter
2. Pretty much does nothing!

First attempt started with a basic training set...

    loPass.inputs  = {1.0,  1.0};
    noFilt.inputs  = {1.0,  0.0};
    hiPass.inputs  = {1.0, -1.0};
    loPass.outputs  = {FilterType::isLoPass};
    noFilt.outputs  = {FilterType::noFilter};
    hiPass.outputs  = {FilterType::isHiPass};
    trainingSet.add (loPass);
    trainingSet.add (noFilt);
    trainingSet.add (hiPass);

But a filter of `{1.0, 0.49}` is classified as 'not a filter', even though this is quite a low pass filter (bass at +4dB, treble at -6dB).

So let's add some more specific training examples, we should only class as 'not a filter' when there is very little tonal change.

    Classification::DataSample loPass2;             // 2. next attempt. if bass/treble difference
    Classification::DataSample hiPass2;             // change is more than ~2dB, classify as filter
    loPass2.inputs = {1.0,  0.1};                   // otherwise say it's pass audio
    hiPass2.inputs = {1.0, -0.1};
    loPass2.outputs = {FilterType::isLoPass};
    hiPass2.outputs = {FilterType::isHiPass};
    trainingSet.add (loPass2);
    trainingSet.add (hiPass2);

Good, but now filters with inverse coefficients don't yield proper results e.g. `{-1.0, -1.0}` should be low pass but is classed as high pass.

Let's add some more training data

    Classification::DataSample loPass3;             // 3. make sure inverted gives correct
    Classification::DataSample hiPass3;             // results
    loPass3.inputs = {-1.0, -1.0};
    hiPass3.inputs = {-1.0,  1.0};
    loPass3.outputs = {FilterType::isLoPass};
    hiPass3.outputs = {FilterType::isHiPass};
    trainingSet.add (loPass3);
    trainingSet.add (hiPass3);
