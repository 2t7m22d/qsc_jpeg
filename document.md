1、读入bin文件，将像素信息分别保存到pixel_R，pixel_G和pixel_B全局数组中。    

2、将RGB颜色空间换为YCrCb颜色空间，提前减少128，存入到pixel_Y，pixel_Cr，pixel_Cb全局数组中。     

3、在接下来部分中需要将数据划分至8x8的矩阵，利用left和up变量标记矩阵最上角的位置，以此来确定矩阵的位置。    

4、调用F函数计算离散余弦变换，输入的两个参数(int u,int v)分别为矩阵最左端（left）与矩阵最上端（up），计算过程中用Sum_Y，Sum_Cr，Sum_Cb函数计算了离散余弦变换公式∑符号后部分，计算结果保存至DCT_Y，DCT_Cr，DCT_Cb全局数组。    

5、以Sum_Y函数为例，(int u,int v,int left,int up)中u，v为被计算到的数字在矩阵中的位置，left和up为矩阵最左上角位置，Sum_Cr，Sum_Cb均如此。     

6、用3一样的方法划分矩阵，然后对应每项除以量化矩阵（QY和QC全局矩阵），这个除法过程用quantization函数实现，其参数同样为被除矩阵的左上角位置（left和up），结果保存到Q_Y,Q_Cr,Q_Cb全局数组中。       

7、用3一样的方法划分矩阵，然后用Zigzag函数实现zigzag排列，其前两个参数依然为矩阵左上角位置，num参数为排列到第几个矩阵（矩阵先从左到右，后从上到下排列），结果保存到Zigzag_Y，Zigzag_Cr和Zigzag_Cb数组中。       

9、Zigzag函数通过判断何时转弯，以及转弯后向哪个方向走实现了zigzag排列（这部分算法参考了 https://blog.csdn.net/PinkFriday/article/details/78275214 ）。       

8、Zigzag后被注释掉部分可以输出zigzag排列结果，结果保存在Zigzag_Y.txt，Zigzag_Cr.txt，Zigzag_Cb.txt中。       

10、后面实现了游程编码，但此时没有用变长编码，而用flag_Y，flag_Cr和flag_Cb全局数组标记了应变长部分，到最后输出时再实现变长编码，游程编码结果保存在RLC_Y,RLC_Cb和RLC_Cr数组中。      

11、judge_EOB_Y、judge_EOB_Cr和judge_EOB_Cb分别用于判断Zigzag_Y[i][j]、Zigzag_Cr[i][j]、Zigzag_Cb[i][j]后面是否全为0，其参数即i和j。      

12、DC_Y_lcnt、DC_C_lcnt、AC_Y_lcnt、AC_C_lcnt全局数组记录了霍夫曼编码表中某一长度的编码个数，DC_Y_code、DC_C_code、AC_Y_code和AC_C_code则记录了编码顺序。         

13、search函数用于查找数字在数组中的位置，其第一个参数为整数，第二个参数为数组指针，其用于查找数字在编码顺序表中的位置。         

14、huffman_print函数用于输出输入数字对应的霍夫曼编码，其参数为在编码顺序表中的位置，该编码顺序表对应lcnt数组指针和输出文件指针。           

15、在huffman_print函数中并不直接计算输出结果，而是先计算其编码对应十进制数，在和其编码长度一起传入binary函数中（第一个参数即对应十进制数，第二个参数为编码长度），binary函数将结果放入字符全局数组Huffman_value中，随后输出数组。      

16、binary函数可以将十进制数转化至二进制并且保留前面的0（通过控制参数bit实现），其实现转化二进制的方式即通过加一的不断累积，加一及进位的过程由add1函数的迭代实现，add1函数的参数表示其计算到第几位。        

17、在输出过程中由于没有进行变长编码，但在AC中不需要进行变长编码的部分数字只要加上需要进行变长编码的部分数字的编码位数就能进行霍夫曼编码，因此只要计算编码位数即可，编码位数用bit函数计算，其参数即输入一个整数。         

18、输出需要进行变长编码部分数字即输出其对应反码，输出反码由RLC_print函数实现，其参数为待变为其反码数字和输出文件指针，过程用binary函数计算二进制数。         

19、至此到霍夫曼编码部分完成，后面文件头时间来不及了qaq