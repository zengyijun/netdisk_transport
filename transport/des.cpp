#include "des.h"
#include <QFile>
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include "mainwindow.h"
#include <QObject>

//初始置换表IP
const int DES::IP_Table[] =
{
    58, 50, 42, 34, 26, 18, 10, 2,  60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,  64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9,  1,  59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,  63, 55, 47, 39, 31, 23, 15, 7,
};

//逆初始置换表IP^-1
const int DES::IPR_Table[] =
{
    40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25
};

//扩充置换表E
const int DES::E_Table[48] =
{
    32,1,2,3,4,5,
    4,5,6,7,8,9,
    8,9,10,11,12,13,
    12,13,14,15,16,17,
    16,17,18,19,20,21,
    20,21,22,23,24,25,
    24,25,26,27,28,29,
    28,29,30,31,32,1
};

//P置换
const int DES::P_Table[32] =
{
    16,7,20,21,
    29,12,28,17,
    1,15,23,26,
    5,18,31,10,
    2,8,24,14,
    32,27,3,9,
    19,13,30,6,
    22,11,4,25
};

//置换选择1
const int DES::PC1_Table[56] =
{
    57,49,41,33,25,17,9,
    1,58,50,42,34,26,18,
    10,2,59,51,43,35,27,
    19,11,3,60,52,44,36,
    63,55,47,39,31,33,15,
    7,62,54,46,38,30,22,
    14,6,61,53,45,37,29,
    21,13,5,28,20,12,4
};

//置换选择2
const int DES::PC2_Table[48] =
{
    14,17,11,24,1,5,
    3,28,15,6,21,10,
    23,19,12,4,26,8,
    16,7,27,20,13,2,
    41,52,31,37,47,55,
    30,40,51,45,33,48,
    44,49,39,56,34,53,
    46,42,50,36,29,32
};

//对左移次数的规定
const int DES::LOOP_Table[16] =
{
    1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};

//S-Box S盒
const int DES::S_Box[8][4][16] =
{
    {   //S1
        {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
        {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
        {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
        {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
    },
    {   //S2
        {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
        {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
        {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
        {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}
    },
    {   //S3
        {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
        {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
        {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
        {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}
    },
    {   //S4
        {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
        {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
        {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
        {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}
    },
    {   //S5
        {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
        {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
        {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
        {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}
    },
    {   //S6
        {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
        {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
        {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
        {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}
    },
    {   //S7
        {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
        {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
        {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
        {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}
    },
    {   //S8
        {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
        {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
        {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
        {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}
    }
};
//加密字符串
QString DES::StrEncryption(const QString str, const QString passwd){
    QByteArray ba = str.toLatin1();//此处保存的是QString类型的字符串按位进行存储后的数据
    QString result;
    result.clear();
    int size = ba.size();//包含字符串尾部的结束符
    char *s;
    //初始化s：初始化有两个角度：1.字符串字符个数（含结束符）刚好是8的倍数，此时s直接分配size大小即可
    if(size % 8 == 0)
        //如果size是8的整数，那么直接创建那么大的s
        s = new char(size);
    else{
        //否则，则要分配与此size最接近的8的倍数个大小。如假设实际字符个数是5，那么需要给s分配8个大小的空间
        int n = size / 8;
        size = (n+1)*8;
        s = new char(size);
        memset(s, 0x00, size);//将s所有位均置0
    }
    strncpy(s, ba.data(), ba.size());//将字符串从QString类型转换成unsigned char类型
    unsigned char input[8];//输入输出
    unsigned char output[8];
    char *p = s;//一个指针变量，用以遍历数组
    for(int i = 0; i < size; i+=8)
    {
        memset((void *)input, 0, 8);
        memset((void *)output, 0, 8);//重新调整input和output的各位数字，将其调整为0
        strncpy((char*)input, p, 8);
        strcat((char*)input, "\0");
        p = p+8;
        //调用正式的加密处理函数。每次加密的数据位数是64位
        if(!RunDesEncryption(input, output, 8, passwd, ECB))
            return "";
        else
        {
            //将此时output中的值以空格为间隔存入QString中
            for(int j = 0; j < (int)sizeof(output); j++)
            {
                //直接保存加密后的数值，而不是将其转换成字符串保存
                result.append(QString::asprintf("%hhu",output[j]));
                result.append(" ");
            }
        }
    }
    delete s;
    return result;//返回加密结果
}
QString DES::StrDecryption(const QString str, const QString passwd){
//此时待解密的子字符是一串以空格分割的数字，直接将这些数字读出并提取到uchar类型的数组中即可
    QStringList strlist = str.split(" ");
    int size = strlist.size()-1;//除去末尾的空格后的实际大小
    unsigned char *s = new unsigned char(size);
    for(int i = 0; i < size; i++)
    {
        s[i] = strlist[i].toInt();
    }
    QString result;//保存解密的结果
    unsigned char input[8];
    unsigned char output[8];
    unsigned char *p = s;
    for(int i = 0; i < size; i+=8)
    {
        memset((void *)input,0,8);
        memset((void *)output,0,8);

        strncpy((char*)input, (char*)p, 8);
        p=p+8;
        if(!RunDesDecryption(input, output, 8, passwd, ECB))
        {
            return "";
        }
        else
            for(int j = 0; j < (int)sizeof(output); j++)
                result.append((char)output[j]);
    }
    delete s;
    QChar h0 = 0x00;
    result.replace(h0, "");
    return result;
}

//加密文件,对文件进行读写操作
bool DES::FileEncryption(const QString FileName, const QString key,int mode, bool rm)
{
    QFile srcfile(FileName), detfile(FileName+".ls");
    unsigned char input[8];
    unsigned char output[8];
    long count;

    if( srcfile.open(QIODevice::ReadOnly) && detfile.open(QIODevice::ReadWrite) )
    {
        while( !srcfile.atEnd() )
        {
            memset((void *)input,0,8);
            memset((void *)output,0,8);
            count = srcfile.read((char*)input, 8);
            if (!RunDesEncryption(input, output, count, key,mode))
            {
//                QMessageBox::critical(NULL, QObject::tr("错误"),QObject::tr("加密失败！"));
                return false;
            }
            detfile.write((char*)output,8);
        }
    }
    else
    {
        //QMessageBox::critical(NULL, QObject::tr("错误"),QObject::tr("文件打开失败！"));
        return false;
    }

    srcfile.close();
    detfile.close();
    if ( rm )
        QFile::remove(FileName);

    return true;

}


//解密文件
bool DES::FileDecryption(const QString FileName, const QString key, int mode, bool rm)
{

    if ( !FileName.endsWith(".ls"))
    {
//        QMessageBox::warning(NULL, QObject::tr("Warning"),QObject::tr("该文件并非本软件加密的文件，或者后缀不为'.ls'！"));
        return false;
    }

    QString FileNameNew = FileName.left( (FileName.length() - 3) );
    QFile srcfile(FileName), detfile(FileNameNew +".de");


    unsigned char input[8];
    unsigned char output[8];

    long count = 0;
    long tempsize = 0;
    long size = 0;

    if( srcfile.open(QIODevice::ReadOnly) && detfile.open(QIODevice::ReadWrite) )
    {
        size = srcfile.size();
        while(!srcfile.atEnd())
        {
            memset((void *)input,0,8);
            memset((void *)output,0,8);
            count = srcfile.read((char*)input, 8);
            tempsize+=8;
            if ( !RunDesDecryption(input, output, count, key,mode))
            {
//                QMessageBox::critical(NULL, QObject::tr("错误"),QObject::tr("解密失败！"));
                return false;
            }
            if (tempsize < size)
            {
                detfile.write((char*)output,count);
            }
            else
            {
                break;
            }
        }
        //下面用于判断是否有填充位，如果有，则不写入文件.
        //这个处理感觉不太妥当，到时候看看有什好方法
        if ( output[7] < 8)
        {
            for(count = 8 - output[7]; count < 7; count++)
            {
                if(output[count] != output[7])
                {
                    break;
                }
            }
        }

        if(count == 7)//有填充
        {
            detfile.write((char*)output,8 - output[7]);
        }
        else//无填充
        {
            detfile.write((char*)output,8);
        }

        if ( !detfile.rename(FileNameNew))//重命名
            return true;
//            QMessageBox::critical(NULL, QObject::tr("错误"),FileNameNew+QObject::tr("原文件存在，请自行对解密后的文件重命名！"));

    }
    else
    {
//        QMessageBox::critical(NULL, QObject::tr("错误"),QObject::tr("文件打开失败！"));
        return false;
    }

    srcfile.close();
    detfile.close();
    if ( rm )
        QFile::remove(FileName);
    return true;
}


//调用RunDes进行加密
bool DES::RunDesEncryption(const unsigned char *input,  unsigned char *output,\
                            unsigned int datalen,const QString key ,int mode)
{
    unsigned keylen = ((key.length()/9)+1)*8 < 24 ? ((key.length()/9)+1)*8 : 24;
    return RunDes(ENCRYPT, mode, input, output, datalen, (unsigned char *)key.toLatin1().data(), keylen);
}


//调用解密函数
bool DES::RunDesDecryption(const unsigned char *input, unsigned char *output,\
                            unsigned int datalen,const QString key ,int mode)
{
//    unsigned count = (datalen/9+1)*8;
    unsigned keylen = ((key.length()/9)+1)*8 < 24 ? ((key.length()/9)+1)*8 : 24;
    return RunDes(DECRYPT, mode, input, output, datalen, (unsigned char *)key.toLatin1().data(), keylen);
}

//执行算法，成功返回true
//密钥(可为8位,16位,24位)支持3密钥
//密钥长度，多出24位部分将被自动裁减
bool DES::RunDes(bool bType,bool bMode,const unsigned char *In,unsigned char *Out,\
                        unsigned datalen,const unsigned char *Key,unsigned keylen)
{

    unsigned char* tempBuf;
    unsigned padlen = datalen;
    unsigned char inbuf[datalen + 8];

    //判断输入合法性
    if(!(In && Out && Key && datalen && keylen>=8))
        return false;

    memset(inbuf, 0x00, sizeof(inbuf));
    memcpy(inbuf, In, datalen);

    //只处理8的整数倍，不足长度自己填充
    if(datalen & 0x00000007)
    {
        if (!RunPad(PAD_PKCS_7, In, datalen, inbuf, padlen))
        {
            return false;
        }
        tempBuf = inbuf;
//        qDebug() << "PAD_PKCS_7";
    }
    else
    {
        tempBuf = inbuf;
    }

    bool m_SubKey[3][16][48];		//秘钥

    //构造并生成SubKeys
    char nKey	=	(keylen>>3)>=3 ? 3: (keylen>>3);
    for(int i=0;i<nKey;i++)
    {
        SetSubKey(&m_SubKey[i],&Key[i<<3]);
    }

    if(bMode == ECB)	//ECB模式
    {
        if(nKey == 1)	//单Key
        {
            for(int i=0,j = padlen>>3; i<j; ++i,Out+=8,tempBuf+=8)
            {
                DES_Unit(Out,tempBuf,&m_SubKey[0],bType);
            }
        }
        else
        if(nKey == 2)	//3DES 2Key
        {
            for(int i=0,j = padlen>>3;i<j;++i,Out+=8,tempBuf+=8)
            {
                DES_Unit(Out,tempBuf,&m_SubKey[0],bType);
                DES_Unit(Out,Out,&m_SubKey[1],!bType);
                DES_Unit(Out,Out,&m_SubKey[0],bType);
            }
        }
        else			//3DES 3Key
        {
            for(int i=0,j=padlen>>3;i<j;++i,Out+=8,tempBuf+=8)
            {
                DES_Unit(Out,tempBuf,&m_SubKey[bType? 2 : 0],bType);
                DES_Unit(Out,Out,&m_SubKey[1],!bType);
                DES_Unit(Out,Out,&m_SubKey[bType? 0 : 2],bType);
            }
        }
    }
    else				//CBC模式
    {
        unsigned char cvec[8] = "";	//扭转向量
        unsigned char cvin[8] = ""; //中间变量

        if(nKey == 1)	//单Key
        {
            for(int i=0,j=padlen>>3;i<j;++i,Out+=8,tempBuf+=8)
            {
                if(bType == ENCRYPT)
                {
                    for(int j=0;j<8;++j)		//将输入与扭转变量异或
                    {
                        cvin[j]	= tempBuf[j] ^ cvec[j];
                    }
                }
                else
                {
                    memcpy(cvin,tempBuf,8);
                }

                DES_Unit(Out,cvin,&m_SubKey[0],bType);

                if(bType == ENCRYPT)
                {
                    memcpy(cvec,Out,8);			//将输出设定为扭转变量
                }
                else
                {
                    for(int j=0;j<8;++j)		//将输出与扭转变量异或
                    {
                        Out[j] = Out[j] ^ cvec[j];
                    }
                    memcpy(cvec,cvin,8);			//将输入设定为扭转变量
                }
            }
        }
        else
        if(nKey == 2)	//3DES CBC 2Key
        {
            for(int i=0,j=padlen>>3;i<j;++i,Out+=8,tempBuf+=8)
            {
                if(bType == ENCRYPT)
                {
                    for(int j=0;j<8;++j)		//将输入与扭转变量异或
                    {
                        cvin[j]	=	tempBuf[j] ^ cvec[j];
                    }
                }
                else
                {
                    memcpy(cvin,tempBuf,8);
                }

                DES_Unit(Out,cvin,&m_SubKey[0],bType);
                DES_Unit(Out,Out,&m_SubKey[1],!bType);
                DES_Unit(Out,Out,&m_SubKey[0],bType);

                if(bType == ENCRYPT)
                {
                    memcpy(cvec,Out,8);			//将输出设定为扭转变量
                }
                else
                {
                    for(int j=0;j<8;++j)		//将输出与扭转变量异或
                    {
                        Out[j]	=	Out[j] ^ cvec[j];
                    }
                    memcpy(cvec,cvin,8);			//将输入设定为扭转变量
                }
            }
        }
        else			//3DES CBC 3Key
        {
            for(int i=0,j=padlen >>3;i<j;++i,Out+=8,tempBuf+=8)
            {
                if(bType ==	ENCRYPT)
                {
                    for(int j=0;j<8;++j)		//将输入与扭转变量异或
                    {
                        cvin[j]	= tempBuf[j] ^ cvec[j];
                    }
                }
                else
                {
                    memcpy(cvin,tempBuf,8);
                }

                DES_Unit(Out,cvin,&m_SubKey[bType ? 2 : 0],bType);
                DES_Unit(Out,Out,&m_SubKey[1],!bType);
                DES_Unit(Out,Out,&m_SubKey[bType ? 0 : 2],bType);

                if(bType == ENCRYPT)
                {
                    memcpy(cvec,Out,8);			//将输出设定为扭转变量
                }
                else
                {
                    for(int j=0;j<8;++j)		//将输出与扭转变量异或
                    {
                        Out[j] = Out[j] ^ cvec[j];
                    }
                    memcpy(cvec,cvin,8);			//将输入设定为扭转变量
                }
            }
        }
    }

    return true;
}

//DES单元运算
void DES::DES_Unit(unsigned char Out[], const unsigned char In[], const PSubKey pSubKey, bool Type)
{
    bool M[64], tmp[32], *Li=&M[0], *Ri=&M[32];
    ByteToBit(M, In, 64);
    Transform(M, M, IP_Table, 64);
    if( Type == ENCRYPT )
    {
        for(int i=0; i<16; ++i)
        {
            memcpy(tmp, Ri, 32);		//Ri[i-1] 保存
            F_func(Ri, (*pSubKey)[i]);	//Ri[i-1]经过转化和SBox输出为P
            Xor(Ri, Li, 32);			//Ri[i] = P XOR Li[i-1]
            memcpy(Li, tmp, 32);		//Li[i] = Ri[i-1]
        }
    }
    else
    {
        for(int i=15; i>=0; --i)
        {
            memcpy(tmp, Ri, 32);		//Ri[i-1] 保存
            F_func(Ri, (*pSubKey)[i]);	//Ri[i-1]经过转化和SBox输出为P
            Xor(Ri, Li, 32);			//Ri[i] = P XOR Li[i-1]
            memcpy(Li, tmp, 32);		//Li[i] = Ri[i-1]
        }
    }
    RotateL(M,64,32);					//Ri与Li换位重组M
    Transform(M, M, IPR_Table, 64);		//最后结果进行转化
    BitToByte(Out, M, 64);				//组织成字符
}


//计算并填充子密钥到SubKey数据中
void DES::SetSubKey(PSubKey pSubKey, const unsigned char Key[])
{
    bool K[64], *KL=&K[0], *KR=&K[28];

    ByteToBit(K, Key, 64);
    Transform(K, K, PC1_Table, 56);

    for(int i=0; i<16; ++i)
    {
        RotateL(KL, 28, LOOP_Table[i]);
        RotateL(KR, 28, LOOP_Table[i]);
        Transform((*pSubKey)[i], K, PC2_Table, 48);
    }
}


//把BYTE转化为Bit流
void DES::ByteToBit(Bit *Out, const unsigned char *In, int bits)
{
    for(int i = 0; i < bits; ++i)
        Out[i] = (In[i>>3]>>(7 - (i&7))) & 1;
}

//把Bit转化为Byte流
void DES::BitToByte(unsigned char *Out, const Bit *In, int bits)
{
    memset(Out, 0, bits>>3);

    for(int i = 0; i < bits; ++i)
        Out[i>>3] |= In[i]<<(7 - (i&7));
}

//把BIT流按位向左迭代
void DES::RotateL(Bit *In, int len, int loop)
{
    Bit Tmp[256];

    memcpy(Tmp, In, loop);
    memcpy(In, In+loop, len-loop);
    memcpy(In+len-loop, Tmp, loop);
}

//把两个Bit流进行异或
void DES::Xor(Bit *InA, const Bit *InB, int len)
{
    for(int i = 0; i < len; ++i)
        InA[i] ^= InB[i];

}

//把两个Bit流按表进行位转化
void DES::Transform(Bit *Out, Bit *In, const int *Table, int len)
{
    bool Tmp[256];

    for(int i=0; i<len; ++i)
        Tmp[i] = In[ Table[i]-1 ];

    memcpy(Out, Tmp, len);
}

//实现数据加密S BOX模块
void DES::S_func(Bit Out[], const Bit In[])
{
    for(int i=0,j,k; i<8; ++i,In+=6,Out+=4)
    {
        j = (In[0]<<1) + In[5];
        k = (In[1]<<3) + (In[2]<<2) + (In[3]<<1) + In[4]; //组织SID下标

        for(int l=0; l<4; ++l)                               //把相应4bit赋值
            Out[l] = (S_Box[i][j][k]>>(3 - l)) & 1;
    }
}

//实现数据加密到输出P
void DES::F_func(Bit In[32], const Bit Ki[48])
{
    bool MR[48];

    Transform(MR, In, E_Table, 48);
    Xor(MR, Ki, 48);
    S_func(In, MR);
    Transform(In, In, P_Table, 32);
}

//对未满8字节的数据进行填充
bool DES::RunPad(int nType, const unsigned char *In, unsigned datalen, unsigned char *Out, unsigned &padlen)
{

    if (In == NULL || datalen == 0 || Out == NULL)
        return false;
    int res = (datalen & 0x00000007);

    if (res == 0)
    {
        padlen = datalen;
        memcpy(Out, In, datalen);
        return true;
    }

    padlen	=	(datalen+8-res);
    memcpy(Out,In,datalen);

    if(nType	==	PAD_PKCS_7)
    {
         memset(Out+datalen,8-res,8-res);//对空的地方填充空字节的总长度
    }
    else
    {
        return false;
    }

    return true;
}
