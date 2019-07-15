# Binomial Model Project: P10_hw_Original

This project represents the original Binomial Model kernel implementation targeting the Alveo card. In this project the entire application is organized as a purely sequential process:   input data transfer from Host to Global Memory, Kernel execution and result transfer from Global to Host memory is done for each single test vector:

Data 1 (Host->Global) => Kernel => Result 1 (Global -> Host) => Data 2 (Host->Global) => Kernel => Result 2 (Global -> Host) => …

Please refer to the [BinomialModel.pdf] document for detailed information regarding design setup, execution and results comparison.

[BinomialModel.pdf]: ../BinomialModel.pdf
