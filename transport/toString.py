# 将输入的一串16进制数字转换成字符串
import sys
def hex_string(str):
    if("0x" in str):
        str = int(str, 16)
        i = str.bit_length() +1
        mes = []
        while i > 0:
            m = str & 0b11111111
            mes.append(chr(m))
            str = str>>8
            i-=8   
        str = ""
        mes.reverse()
        for i in mes:
            str += i
        return str
        

if __name__=='__main__':
    input = sys.argv[1]
    output = hex_string(input)
    print(output)
    