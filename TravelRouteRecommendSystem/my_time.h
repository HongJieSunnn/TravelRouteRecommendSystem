#ifndef MY_TIME_H
#define MY_TIME_H
namespace
{
	enum TimeFormatEnum
	{
		YYYY_MM_DD_HH_MM = 1,
		HH_MM
	};
}

/*
	ʱ��ṹ ��������ʱ�ַֿ� ���ṩ������������MyTime���ʱ��(����)
	cost_time���ַ�����ʽ������ ������
*/
class MyTime
{
public:
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int format;

	/*
		��������ʱ���ļ�� ���ص�λ����
		*time:Ҫ�����ʱ���
	*/
	int costTime(MyTime time);

	/*
		��MyTime����ת����tm�����Ա����
	*/
	tm myTimeToTmTime(MyTime time);

	/*
		costTime��һ��������ʽ ��Ҫ����ʱ��
		*start_time:��ʼʱ��
		*end_time:����ʱ��
	*/
	static int costTime(MyTime start_time, MyTime end_time);

	/*
		��ʱ���ַ���ת��MyTime�ṹ
		*time:yyyy.mm.dd hh:mm��ʽ���ַ���/hh:mm��ʽ
	*/
	static MyTime stringToMyTime(string time, int time_format);

	/*
		ͨ�����ݿ����cost_time�ֶλ������ʱ��
		*cost_time:����ʱ����ַ���ʾ
	*/
	static int getCostTimeByString(string cost_time);

	/*
		�ж�ʱ���Ƿ����ʱ���ʽ
		*time:ʱ����ַ���ʾ
		�����жϷ��ż��Ƿ�Ϊ���� ��ΪĬ�ϱ����� �����ط��ᴦ���
	*/
	static bool conformTimeFormatOrNot(string time,int time_format);
};

#endif // !MY_TIME_H