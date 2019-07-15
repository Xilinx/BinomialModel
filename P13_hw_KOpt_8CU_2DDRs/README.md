# Binomial Model Project: P13_hw_KOpt_8CU_2DDRs

Another method to further accelerate your application is to instantiate multiple compute units (CU) on Alveo card. In addition, connecting different compute units to different DDR banks can be very beneficial to improve the performance if significant amount of data should be transferred between compute units and Global Memory.

This project demonstrates the both techniques, by implementing 8 CUs: 4 CUs are connected to the DDR[0] and another 4 CUs are connected to the DDR[3] banks.

Please refer to the [BinomialModel.pdf] document for detailed information regarding design setup, execution and results comparison.

[BinomialModel.pdf]: ../BinomialModel.pdf
