#ifndef VEHICLE_H
#define VEHICLE_H

//����������ʾ��ͨ���߶��� ��������һЩ��ͨ���ߵ���Ϣ
//���Ǹ����� ����Ľ�ͨ����Ҫ�̳���
class Vehicle
{
public:
protected:
	//���캯�� protected ������ֱ�Ӵ����������
	Vehicle();
};

class AirPlane:public Vehicle
{
public:
	AirPlane();
	~AirPlane();

private:

};

class HSRC:public Vehicle
{
public:
	HSRC();
	~HSRC();

private:

};

#endif // !VEHICLE_H