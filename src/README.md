
# ebc�����ĵ�
## ���뻷��
 VC2008(9.0)��ʹ�������汾���ܻᵼ��DLL�汾��һ�£����д�������⣻
## ���������ļ�����ʹ��VC2008����󷽱�ʹ�ã�
+ [boost 1.55](http://www.boost.org/)
+ [cef_binary_3.2272.32.gbda8dc7_windows32](https://bitbucket.org/chromiumembedded/cef)

## cef_binary_3.2272.32.gbda8dc7_windows32\include\internal\cef_types.h �ļ�91��λ�ã������������д��룺
 #ifndef uint32
 
 #define uint32 unsigned int
 
 #endif
 
 typedef uint32              cef_color_t;

## \src\POP\cefĿ¼�� client_handler.h,client_handler.cpp ע�͵� virtual bool OnFileDialog ������

## �½� \src\build Ŀ¼�����ƶ����ͻ��˰�װĿ¼ ebcontrol.dll,eboleimage.dll �����ļ�����Ŀ¼�£�
## ���� boost �� cef �����������⣬������Ҫ�õ��� .h ͷ�ļ��� .lib ���ļ���ȫ���� \src Ŀ¼�£���������ӵ�VC�������棻
## �����ʱ����ѡ����� Debug �� Release ����������Ҫ���룻
## ����ɹ����� ebc.exe �ļ��������ͻ��ˣ��滻�ļ������������Ƿ�ɹ���
 