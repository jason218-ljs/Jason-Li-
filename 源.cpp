#define _CRT_SECURE_NO_WARNINGS//让ctime不报错
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>//sort(),min()
#include <sstream>//字符串流stringstream（拆分字符串）
#include <conio.h>//键盘不回显输入,_getch()：密码不显示输入
#include <cstdlib>//system("cls")清屏	("pause")暂停	rand()随机数	remove()删除文件
#include <ctime>//time()获取当前时间	ctime()时间转字符串	srand((unsigned)time(0))设置随机种子
#include <windows.h>//Sleep()延时	SetConsoleTextAttribute()设置颜色	GetStdHandle()获取控制台句柄
#include <chrono>
#include <iomanip>//setprecision()设置小数位数	fixed固定小数格式

using namespace std;

//================================创建数据==============================

//单个用户的信息
struct info
{
	string name;
	string password;
};
//用户排行榜信息
struct record {
	string name;
	int wpm;//每分钟输入单词数
};
//创建程序中的用户信息列表，避免频繁的文件打开和关闭
vector<info> userlist;

//================================CSS函数==============================

//设置颜色
void set_color(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);//给控制台文字设置颜色
	//SetConsoleTextAttribute系统API
	// GetStdHandle(STD_OUTPUT_HANDLE)获取控制台窗口的“输出句柄”，让系统知道你要改哪里的颜色
}

/*
颜色数据
- 0 = 黑​
- 1 = 蓝​
- 2 = 绿​
- 3 = 浅蓝​
- 4 = 红​
- 5 = 紫​
- 6 = 黄​
- 7 = 白（默认）​
- 8 = 灰​
- 9 = 浅蓝​
- 10 = 浅绿​
- 11 = 亮青​
- 12 = 亮红​
- 13 = 亮紫​
- 14 = 亮黄​
- 15 = 亮白
*/

//================================安全输入函数==============================

//密码输入不显示明文只显示*
string input_password()
{
	string password;
	char ch;
	while (true)
	{
		ch = _getch();//不回显读取字符(也就是直接读取键盘上的按键）
		//输入回车或换行就结束
		if (ch == '\r' || ch == '\n') break;
		//按下退格键时
		if (ch == '\b' && !password.empty())//密码不为空才能删
		{
			password.pop_back();//把字符串最后一个字符删掉
			//一开始\b ：光标左移一格	然后输出空格：把原来的 * 覆盖掉	最后再\b ：光标停在正确位置最终效果：屏幕上的 * 少一个
			cout << "\b \b"; // 删掉屏幕上的 *
		}
		// 正常字符
		else if (ch != '\b')
		{
			password += ch;
			cout << "*";
		}
	}
	cout << endl;//防止后面程序未换行
	return password;
}

//安全输入菜单选项,输入字母或汉字也不会崩
int input_menu()
{
	int op;
	while (!(cin >> op))//输入失败返回false
	{
		cin.clear();//清除cin的错误状态，防止其卡死
		cin.ignore(1000, '\n'); //将这行中的垃圾信息清除，再让用户重新输入
		set_color(4);
		cout << "输入无效，请输入有效数字：";
		set_color(7);
	}
	return op;
}

//================================注册登录修改密码删除用户函数==============================

//设置每个用户单独的存储文件
string user_record_file(string username)
{
	return username + "_record.txt";
}

//加载文件数据到程序中的函数
void load_file()
{
	//尝试打开文件
	ifstream fin("user.txt");
	if (!fin)
	{
		set_color(4);
		cout << "未找到文件，将创建新文件" << endl; 
		set_color(7);
		return;
	}
	//从文件中逐行读取内容
	string line;
	while (getline(fin, line))
	{
		if (line.empty()) continue;//防止遇到空行时出现读写错误
		stringstream ss(line);//帮这一行放入字符串流ss（类似控制台），方便后续像cin一样拆分
		info user;
		ss >> user.name >> user.password;
		userlist.push_back(user);
	}
	fin.close();//读取文件结束后记得要关闭
}

//保存数据到文件中的函数
void save_to_file()
{
	ofstream fout("user.txt");
	if (!fout)
	{
		set_color(4);
		cout << "文件保存失败！" << endl;
		set_color(7);
		return;
	}
	for (auto& user : userlist)
	{
		fout << user.name << " " << user.password << endl;//保存文件时注意其中的空格和换行
	}
	fout.close();
}

//注册
void register_user()
{
	info user;
	//处理用户名
	while (true)
	{
		cout << "请输入用户名";
		cin >> user.name;
		bool repeat = false;
		for (auto& finduser : userlist)
		{
			if (finduser.name == user.name)
			{
				repeat = true;
				break;
			}
		}
		if (!repeat) break;
		set_color(4);
		cout << "用户名重复了" << endl;
		set_color(15);
		cout << "重新输入请按1，退出按0：" << endl;
		set_color(7);
		int op = input_menu();
		if (op == 0) return;
	}
	//设置新密码
	string password, re_password;
	while (true)
	{
		cout << "输入密码：";
		password = input_password();
		//对密码长度进行限制
		while (password.size() < 6) 
		{
			set_color(4);
			cout << "密码长度至少6位，请重新输入！" << endl;
			set_color(7);
			cout << "输入密码：";
			password = input_password();
		}
		set_color(15);
		cout << "再次输入密码：";
		re_password = input_password();
		if (password == re_password) break;
		set_color(4);
		cout << "两次不一致！重试1，退出0：" << endl;
		set_color(7);
		int op = input_menu();
		if (op == 0) return;
	}
	user.password = password;
	userlist.push_back(user);
	save_to_file();
	set_color(2);
	cout << "注册成功！" << endl;
	set_color(7);
	system("pause");
}

//登录
string login_user()
{
	string name;
	info* user = nullptr;
	//处理用户名
	while (true)
	{
		cout << "请输入用户名";
		cin >> name;
		user = nullptr;//注意重新赋值
		for (auto& finduser : userlist)
		{
			if (finduser.name == name)
			{
				user = &finduser;
				break;
			}
		}
		if (user != nullptr) break;
		set_color(4);
		cout << "用户不存在，重新输入请按1，退出按0：" << endl;
		set_color(7);
		int op = input_menu();
		if (op == 0) return "";
	}
	//验证密码
	string password;
	while (true)
	{
		cout << "请输入密码";
		password = input_password();
		if (user->password == password)
		{
			break;
		}
		set_color(4);
		cout << "密码错误，重新输入请按1，退出按0：" << endl;
		set_color(7);
		int op = input_menu();
		if (op == 0) return "";
	}
	set_color(2);
	cout << "登录成功！欢迎你，" << name << endl;
	set_color(7);
	system("pause");
	return name;
}

//修改密码
void modify_user_password()
{
	string name;
	info* user = nullptr;
	//处理用户名（同登录）
	while (true)
	{
		cout << "输入用户名：";
		cin >> name;
		user = nullptr;//注意重新赋值
		for (auto& finduser : userlist)
		{
			if (finduser.name == name)
			{
				user = &finduser;
				break;
			}
		}
		if (user != nullptr) break;
		set_color(4);
		cout << "用户不存在，重新输入请按1，退出按0：" << endl;
		set_color(7);
		int op = input_menu();
		if (op == 0) return;
	}
	//验证旧密码（同登录）
	string old_password;
	while (true)
	{
		cout << "输入旧密码：";
		old_password = input_password();
		if (user->password == old_password)
		{
			break;
		}
		set_color(4);
		cout << "旧密码错误，重新输入请按1，退出按0：" << endl;
		set_color(7);
		int op = input_menu();
		if (op == 0) return;
	}
	//设置新密码(类似注册）
	string new_password, re_password;
	while (true)
	{
		cout << "输入新密码：";
		new_password = input_password();
		while (new_password.size() < 6 || new_password == old_password)
		{
			if (new_password.size() < 6)
			{
				set_color(4);
				cout << "密码至少6位！\n";
				set_color(7);
			}
			else if (new_password == old_password)
			{
				set_color(4);
				cout << "新密码不能与旧密码相同！\n";
				set_color(7);
			}
			cout << "请重新输入新密码：";
			new_password = input_password();
		}
		set_color(15);
		cout << "再次输入密码：";
		set_color(7);
		re_password = input_password();
		if (new_password == re_password) break;
		set_color(4);
		cout << "两次不一致！重试1，退出0：" << endl;
		set_color(7);
		int op = input_menu();
		if (op == 0) return;
	}
	user->password = new_password;
	save_to_file();
	set_color(2);
	cout << "修改成功！" << endl;
	set_color(7);
	system("pause");
}

//删除账户
void delete_user()
{
	string name;
	info* user = nullptr;
	user = nullptr;//注意重新赋值
	//处理用户名（同登录）
	while (true)
	{
		cout << "输入要删除的用户名：";
		cin >> name;
		for (auto& finduser : userlist)
		{
			if (finduser.name == name)
			{
				user = &finduser;
				break;
			}
		}
		if (user != nullptr) break;
		set_color(4);
		cout << "用户不存在，重新输入请按1，退出按0：" << endl;
		set_color(7);
		int op = input_menu();
		if (op == 0) return;
	}
	//验证密码（同登录）
	string password;
	while (true)
	{
		cout << "输入密码：";
		password = input_password();
		if (user->password == password)
		{
			//二次确认
			cout << "确定要删除该用户及所有历史记录吗？(Y/N)：";
			char confirm;
			cin >> confirm;
			if (!(confirm == 'Y' || confirm == 'y'))
			{
				set_color(4);
				cout << "已取消删除" << endl;
				set_color(7);
				system("pause");
				return;
			}
			break;
		}
		set_color(4);
		cout << "密码错误，重新输入请按1，退出按0：" << endl;
		set_color(7);
		int op = input_menu();
		if (op == 0) return;
	}
	//删除记录
	string record_file = user_record_file(name);
	remove(record_file.c_str()); //删除历史练习记录文件
	vector<info>newuserlist;
	for (auto& u : userlist)
	{
		if (u.name != name)
		{
			newuserlist.push_back(u);
		}
	}
	userlist = newuserlist;
	save_to_file();
	set_color(2);
	cout << "账户与历史记录已全部删除成功！" << endl;
	set_color(7);
	system("pause");
}

//管理员功能函数
void admin_panel()
{
	//验证密码
	while (true)
	{
		system("cls");
		cout << "===== 管理员登录 =====\n";
		string password;
		cout << "请输入管理员密码：";
		password = input_password();
		if (password == "admin")
		{
			set_color(2);
			cout << "\n管理员登录成功\n";
			set_color(7);
			Sleep(1000);
			break;
		}
		else
		{
			set_color(4);
			cout << "\n密码错误，重新输入请按1，退出按0：";
			set_color(7);
			int op = input_menu();
			if (op == 0) return;
		}
	}
	//管理员独立主界面
	while (true)
	{
		system("cls");
		set_color(11);
		cout << "===============================\n";
		cout << "        管理员控制面板        \n";
		cout << "===============================\n";
		set_color(7);
		cout << " 1. 查看所有用户\n";
		cout << " 2. 添加新用户\n";
		cout << " 3. 删除指定用户\n";
		cout << " 4. 清空所有用户\n";
		cout << " 0. 返回主菜单\n";
		cout << "===============================\n";
		cout << "请选择操作：";
		int ad_op = input_menu();
		if (ad_op == 1)
		{
			system("cls");
			cout << "===== 用户列表 =====\n";
			if (userlist.empty()) cout << "暂无用户" << endl;
			else
			{
				for (auto& user : userlist)
				{
					cout << "账户名：" << user.name << "\t密码：" << user.password << endl;
				}
			}
			cout << endl;
			system("pause");
		}
		else if (ad_op == 2)
		{
			system("cls");
			register_user();
		}
		else if (ad_op == 3)
		{
			system("cls");
			cout << "现有用户：";
			for (auto& user : userlist) cout << user.name << " ";
			cout << endl;
			delete_user();
		}
		else if (ad_op == 4)
		{
			system("cls");
			cout << "确定清空所有用户？(Y/N)：";
			char c;
			cin >> c;
			if (c == 'Y' || c == 'y')
			{
				for (auto& user : userlist)
				{
					string f = user_record_file(user.name);//返回文件路径名
					remove(f.c_str());//把string转成char*后移除
				}
				userlist.clear();//清除所有用户后保存
				save_to_file();
				set_color(2);
				cout << "已清空所有用户！\n";
				set_color(7);
			}
			else cout << "已取消\n";
			system("pause");
		}
		else if (ad_op == 0)
		{
			break;//返回主菜单
		}
		else
		{
			cout << "输入无效！" << endl;//防止输入其他数字
			system("pause");
		}
	}
}

//结束界面
void end_menu()
{
	set_color(2);
	cout << "欢迎您再次使用打字练习小程序,期待与您再次相会" << endl;
	set_color(7);
	Sleep(1000);
	//倒计时3秒，每秒刷新数字
	for (int i = 3; i >= 1; --i)
	{
		system("cls");
		//把光标回到上一行，覆盖数字，实现倒计时动效
		cout << "倒计时：" << i << "s后结束程序...\n";
		Sleep(1000); // 暂停1秒
	}
}

//打印菜单函数
void print_menu()
{
	system("cls");
	set_color(15);
	cout << "\n===== 菜单 =====" << endl;
	cout << "1. 注册" << endl;
	cout << "2. 登录" << endl;
	cout << "3. 删除账户" << endl;
	cout << "4. 修改密码" << endl;
	cout << "5. 管理员登录\n";
	cout << "0. 退出" << endl;
	cout << "请选择：";
	set_color(7);
}

//================================小程序功能函数==============================

//练习使用的文本（分难度）
vector<string> easy_articles = {
	"Hello world.",
	"I am happy.",
	"Good good study.",
	"Day day up."
};

vector<string> mid_articles = {
	"Life is like riding a bicycle.",
	"To keep your balance you must keep moving.",
	"Believe you can and you are halfway there."
};

vector<string> hard_articles = {
	"Success is not final, failure is not fatal, it is the courage to continue that counts.",
	"The only way to do great work is to love what you do.",
	"You miss 100 percent of the shots you don't take."
};

//每分钟输入单词数，用WPM指标来衡量
int calculate_WPM(const string& input, double seconds) 
{
	int words = input.size() / 5;//国际标准，五个字符算作一个单词
	return seconds > 0 ? (int)(words * 60.0 / seconds) : 0;//防止除0造成程序崩溃(似乎可以不要，因为时间一定大于0）
}

//打字练习功能主程序
void practice_typing(string username)
{
	system("cls");
	int level;
	cout << "===== 选择难度 =====\n";
	cout << "1.简单（短句）\n";
	cout << "2.中等（中句）\n";
	cout << "3.困难（长句）\n";
	cout << "请选择：";
	level = input_menu();

	string level_name;
	string text;
	if (level == 1)
	{
		text = easy_articles[rand() % easy_articles.size()];
		level_name = "简单";
	}
	else if (level == 2)
	{
		text = mid_articles[rand() % mid_articles.size()];
		level_name = "中等";
	}
	else
	{
		text = hard_articles[rand() % hard_articles.size()];
		level_name = "困难";
	}
	system("cls");

	set_color(11);
	cout << "===== 打字练习 =====\n";
	set_color(7);
	cout << "提示：打对绿色，打错红色，不支持回退\n";
	cout << "-------------------------\n";

	set_color(14);
	cout << text << "\n";
	set_color(7);
	cout << "-------------------------\n";
	cout << "开始输入：" << endl;

	auto start = chrono::steady_clock::now();//记录开始时间

	string user_input;
	int len = text.size();//文本长度

	for (int i = 0; i < len; ++i)
	{
		char c = _getch();
		//禁止回退
		if (c == '\b')
		{
			i--;//因为还要i++，所有提前减一
			continue;
		}
		//回车结束
		if (c == '\r' || c == '\n') break;
		//对符合条件的字符做CSS处理
		user_input += c;
		// 实时颜色
		if (c == text[i]) set_color(10);//正确
		else set_color(12);//错误
		cout << c;
		set_color(7);
	}

	auto end = chrono::steady_clock::now(); //记录结束时间
	chrono::duration<double> dur = end - start;//把时间差转换成以秒为单位的时间段
	double sec = dur.count();//把这段时间提取成一个普通小数(转成数字后才能计算）
	
	int correct = 0;
	int input_len = user_input.size();
	int text_len = text.size();
	for (int i = 0; i < min(input_len, text_len); ++i)
	{
		if (user_input[i] == text[i]) correct++;
	}
	//计算正确率，完成率和wpm
	double acc = 0.0;
	if (input_len > 0)
	{
		acc = (double)correct / input_len * 100;
	}
	double finish_rate = (double)input_len / text_len * 100;
	int wpm = calculate_WPM(user_input, sec);

	//时间戳
	time_t now = time(0);//获取当前系统时间
	string time_str = ctime(&now);//把时间转换成能看懂的时间
	time_str.pop_back();//删除末尾换行符

	set_color(10);
	cout << "\n===== 成绩 =====\n";
	set_color(15);
	cout << "时间: " << time_str << "\n";
	cout << "难度: " << level_name << "\n";
	cout << "用时: " << fixed << setprecision(1) << sec << "s\n";
	cout << "正确率: " << fixed << setprecision(1) << acc << "%\n";
	cout << "完成率: " << fixed << setprecision(1) << finish_rate << "%\n";
	cout << "速度: " << wpm << " wpm\n";
	set_color(7);

	// 保存记录
	ofstream fout(user_record_file(username), ios::app);//注意要用添加模式，以免清除了之前的记录
	fout << "===== 练习记录 =====" << endl;
	fout << "时间: " << time_str << endl;
	fout << "难度: " << level_name << endl;
	fout << "用时: " << fixed << setprecision(1) << sec << "s" << endl;
	fout << "正确率: " << fixed << setprecision(1) << acc << "%" << endl;
	fout << "完成率: " << fixed << setprecision(1) << finish_rate << "%" << endl;
	fout << "wpm: " << wpm << endl;
	fout << "========================================" << endl << endl;
	fout.close();

	system("pause");
}

//查看打字的历史练习记录
void show_history(string username)
{
	system("cls");
	ifstream fin(username + "_record.txt");
	set_color(14);
	cout << "===== 你的历史记录 =====\n";
	set_color(7);

	string line;
	if(!fin) cout << "暂无记录\n";
	else
	{
		while (getline(fin, line))
		{
			cout << line << endl;
		}
	}
	fin.close();
	system("pause");//1.暂停程序2.在屏幕上显示一行：请按任意键继续...3. 按任意键后，程序才继续往下走
}

//查看当前排名
void rank_list()
{
	system("cls");
	vector<record> ranks;//排名数组
	for (auto& user : userlist)//查找每个用户的最好成绩
	{
		ifstream fin(user.name + "_record.txt");
		if (!fin) continue;
		string line;
		int max_WPM = 0;
		while (getline(fin, line))
		{
			size_t pos = line.find("wpm:");//查找函数的返回类型最好用size_t
			if (pos != string::npos)//如果找到了
			{
				//stoi把截取到的字符串转成int整数
				int WPM = stoi(line.substr(pos + 4));//从pos往后的第四个字符开始截取，一直截取到字符串末尾
				if (WPM > max_WPM) max_WPM = WPM;
			}
		}
		if (max_WPM > 0) ranks.push_back({ user.name,max_WPM });//注意使用的是大括号来添加
	}
	set_color(13);
	cout << "===== 全用户打字排行榜 =====\n";
	set_color(7);
	if (ranks.empty())
	{
		cout << "暂无任何用户的打字记录\n";
		system("pause");
		return;
	}

	//排序:[](record a, record b)使用Lambda表达式，临时写一个函数，比较两个元素大小
	sort(ranks.begin(), ranks.end(), [](record a, record b)
		{
			return a.wpm > b.wpm;//降序排列
		});
	set_color(11);
	cout << "排名\t用户名\t最高WPM\n";
	set_color(15);
	for (int i = 0; i < ranks.size(); i++) 
	{
		cout << i + 1 << "\t" << ranks[i].name << "\t" << ranks[i].wpm << endl;//输出排名
	}
	set_color(7);
	system("pause");
}

//主界面
void user_menu(string username)
{
	while (true) 
	{
		system("cls");
		set_color(14);
		cout << "===== 用户中心 =====\n";
		set_color(2);
		cout << "欢迎回来：" << username << "!\n";
		set_color(15);
		cout << "1. 随机文章练习\n";
		cout << "2. 查看我的记录\n";
		cout << "3. 全用户排行榜\n";
		cout << "0. 退出登录\n";
		set_color(7);
		cout << "请选择：";

		int op = input_menu();
		if (op == 1) practice_typing(username);
		else if (op == 2) show_history(username);
		else if (op == 3) rank_list();
		else if (op == 0)
		{
			//倒计时3秒，每秒刷新数字
			for (int i = 3; i >= 1; --i) 
			{
				system("cls");
				// 把光标回到上一行，覆盖数字，实现倒计时动效
				cout << "倒计时：" << i << "s后返回主页面...\n";
				Sleep(1000); // 暂停1秒
			}
			break;
		}
	}
}

//================================程序入口==============================
int main()
{
	//设置随机数种子，让每次运行程序，随机选的文章都不一样
	srand((unsigned int)time(0));
	system("cls");
	load_file(); // 启动先加载文件
	while (true)
	{
		print_menu();//打印菜单

		int op = input_menu();  // 安全输入
		if (op == 1)
		{
			register_user();
		}
		else if (op == 2)
		{
			string name = login_user();
			if (!name.empty())
				user_menu(name);
		}
		else if (op == 3)
		{
			delete_user();
		}
		else if (op == 4)
		{
			modify_user_password();
		}
		else if (op == 5)
		{
			admin_panel();
		}
		else if (op == 0)
		{
			end_menu();
			return 0;
		}
		else
		{
			set_color(4);
			cout << "请输入有效序号！" << endl;
			set_color(7);
		}
	}
}