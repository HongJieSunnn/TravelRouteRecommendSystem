#ifndef INITMYSQL_H
#define INITMYSQL_H
/*
���ڸ���Ŀ��Ҫʹ�õ�mysql���ݿ� ������Ҫ��ʼ��һ��ȫ�ֵ�mysql�������������ݿ���в���
��main��������ù��캯����ʼ�����ݿ� Ȼ��Ϳ���ֱ��ʹ��
���þ�̬��Ա��������ȫ�ֱ���û���
����
int main()
{
	InitMySQL::initGlobleMySQL("127.0.0.1","root","password","database_name",3306,*"gbk"(��ʡ��)*);
}
*/
class InitMySQL
{
private:
	//mysqlʵ��
	static MYSQL db;
	//sql���ִ�н��ʵ��
	static MYSQL_RES* result;
	/*
		�رյ�ǰ��db ���Ժ�resetGlobleMySQL���ʹ��
	*/
	static CloseStatue closeGlobleMySQL();
public:
	InitMySQL();
	/*
		��ʼ�����ݿ�ʵ���ľ�̬��Ա���� �������ݿ�ֻ��Ҫ��ʼ��һ�ξͿ����������κεط�ʹ��
		*host mysql��ַ
		*user_name ��½�˻�
		*password ����
		*database_name Ҫ�򿪵��Ǹ����ݿ�����
		*port �˿ں�
		*character_set �ַ��� c++Ĭ�� gbk �����ݿ�һ��Ĭ��utf-8 �����û��������
	*/
	static InitStatue initGlobleMySQL(string host, string user_name, string password, string database_name,int port,string character_set="gbk");

	/*
		��������mysqlʵ��
	*/
	static int resetGlobleMySQL(string host, string user_name, string password, string database_name, int port, string character_set = "gbk");

	/*
		�����ط���ȡdbʵ��
		����db�����ָ��
	*/
	static MYSQL* DB();

	/*
		ͨ��SQL�����ִ�н��
		���ؾ�̬���� result 
		�������Ա��ⷵ����ʱָ�����⣬Ҳ���Լ��ٿռ��˷�
	*/
	static MYSQL_RES* execSQLToGetResult(string SQL);

	//~InitMySQL();
};

#endif // !INITMYSQL_H