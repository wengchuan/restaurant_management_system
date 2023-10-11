//The GenerateReport function to generate the pdf need to install the library for making the pdf. The libraray is called libharu. 
//This program is using vcpkg tools to install the libharu library
#define NOMINMAX
#include <iostream>
#include <mysql.h> 
#include <conio.h>
#include <iomanip>
#include <string>
#include <ctime>  
#include <sstream>
#include "hpdf.h"
#include <setjmp.h>
#include <zlib.h>
#include<windows.h>
#include <cstring>
#include <algorithm> // for std::max_element
#include <filesystem> //for get the file directory
using namespace std;

namespace fs = std::filesystem; //for get the file directory
//code for pdf function 
jmp_buf env;
#ifdef HPDF_DLL
void  __stdcall
#else
void
#endif
error_handler(HPDF_STATUS   error_no,
	HPDF_STATUS   detail_no,
	void* user_data)
{
	printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
		(HPDF_UINT)detail_no);
	longjmp(env, 1);
}

//set the console color for bar chart 
void setconsolecolor(int textColor, int bgColor)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (textColor +
		(bgColor * 16)));
}



void Login();
//Admin Module 
void AdminMenu();
void StaffMenu();
//manage user module and function 
void ManageUser();
void AddUser();
void ResetPassword();
void DeleteUser();

//record revenue module and function
void RecordRevenue();
void ViewRevenue();
void CalculateRevenue();
void GenerateReport();
//Staff Module
void ManageInventory();
void ViewItemHistory();
void ViewInventory();
void AddItem();
void UpdateQuantity();
void DeleteItem();
void ResetPasswordStaff();
void Quit();
//get the last date of the month
int GetLastDate(int year, int month) {
	int lastDay;
	if (month == 2) {
		// For February, check if the year is a leap year
		if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
			// Leap year - February has 29 days
			lastDay = 29;
		}
		else {
			// Not a leap year - February has 28 days
			lastDay = 28;
		}
	}
	else if (month == 4 || month == 6 || month == 9 || month == 11) {
		// These months have 30 days
		lastDay = 30;
	}
	else {
		// All other months have 31 days
		lastDay = 31;
	}

	return lastDay;

}
void getCurrentAndPreviousMonths(int& currentYear, int& currentMonth, int previousMonths[], int previousYears[], int numOfPreviousMonths) {
	time_t currentTime;
	struct tm localTime;

	time(&currentTime); // Get the current time
	localtime_s(&localTime, &currentTime); // Convert the current time to the local time

	currentMonth = localTime.tm_mon + 1; // tm_mon is in the range 0-11, so we add 1 to get the actual month
	currentYear = localTime.tm_year + 1900; // tm_year is the number of years since 1900, so we add 1900 to get the actual year
	for (int i = 0; i < numOfPreviousMonths; i++) {
		previousMonths[i] = currentMonth - (i + 1);
		previousYears[i] = currentYear;
		if (previousMonths[i] < 1) {
			previousMonths[i] += 12;
			previousYears[i] -= 1;
		}

	}
}
int qstate;
string userId;
MYSQL* conn;
MYSQL_ROW row;
MYSQL_RES* res;



class db_response //connect to the database 
{
public:
	static void ConnectionFunction()
	{
		conn = mysql_init(0);
		if (!conn)
			cout << "Failed To Connect!" << endl;

		conn = mysql_real_connect(conn, "localhost", "root", "", "restaurant management system", 3306, NULL, 0);
		if (!conn) //checking connected to the table or not 
			cout << "Failed To Connect!" << endl;
	}
};




int main() {

	cout << "Welcome To Restaurant Management System\n";
	db_response::ConnectionFunction();

	Login();
	return 0;



}



void Login() {


	string password;
	string username;

	string role;
	cout << "--- Login Page ---" << endl;
	cout << "Username : ";
	cin >> username;
	cout << "Password : ";
	char ch;
	while ((ch = _getch()) != 13)
	{
		password += ch;
		cout << '*';
	}

	string checkUser_query = "select userId,role from user where username = '" + username + "' and password =SHA1( '" + password + "') and user_deleted =0 ";
	const char* cu = checkUser_query.c_str();
	qstate = mysql_query(conn, cu);

	if (!qstate)
	{
		res = mysql_store_result(conn);

		if (res->row_count == 1)
		{


			while (row = mysql_fetch_row(res)) {
				userId = row[0];
				role = row[1];
			}




			if (role == "1") { // if role 1 is admin, role 2 is staff
				AdminMenu();
			}
			else if (role == "2") {
				StaffMenu();
			}
		}
		else
		{
			char c;
			cout << "\nInvalid username or password. \nWant to try again? (y/n): ";
			cin >> c;
			if (c == 'y' || c == 'Y')
				Login();
			else
				Quit();
		}
	}
	else {
		cout << "Query Execution Problem!" << mysql_errno(conn) << endl;
	}
}


void AdminMenu() {
	int choice, isChoosed = 0;
	system("cls");

	cout << "Admin Menu\n";
	cout << "Select Module\n";
	cout << "1. Manage User Module \n2. Record Revenue \n3. Generate Revenue Report \n4. Quit\n  ";
	//it will loop if the user didn't make the correct choice
	while (isChoosed == 0) {
		cout << "Please Enter 1-4: ";
		cin >> choice;
		if (!cin) {
			cout << "Invalid format!!! Try Again!!!\n";

			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		else {
			switch (choice)
			{
			case 1:
				ManageUser();
				isChoosed = 1;
				break;

			case 2:
				RecordRevenue();
				isChoosed = 1;
				break;
			case 3:
				GenerateReport();
				isChoosed = 1;
				break;
			case 4:
				Quit();
				isChoosed = 1;
				break;

			default:
				cout << "You entered incorrect number.\n Please Try Again\n";
			}
		}
	}

}



void ManageUser() {
	int choice, isChoosed = 0;

	system("cls");
	cout << "Manage User Module\n\n";
	cout << "Select Function\n\n";

	cout << "1. Add User \n2. Reset User Password\n3. Delete User\n4. Quit\n";
	while (isChoosed == 0) {
		cout << "Please Enter 1-4: ";
		cin >> choice;
		if (!cin) {
			cout << "Invalid format!!! Try Again!!!\n";

			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		else {
			switch (choice)
			{
			case 1:
				isChoosed = 1;
				AddUser();
				break;

			case 2:
				isChoosed = 1;
				ResetPassword();
				break;

			case 3:
				isChoosed = 1;
				DeleteUser();
				break;

			case 4:
				isChoosed = 1;
				AdminMenu();
				break;

			default:
				cout << "You entered incorrect number.\n Please Try Again \n";
				break;
			}
		}
	}

}

void AddUser()
{
	string username, password, password2;
	char choice;
	bool isChoosed = false, is_pass_len = false, is_pass_correct = false, user_existed, user_deleted;
	system("cls");
	cout << "Add User\n\n";
	cout << "1. Add User\n2. Quit\n";
	while (!isChoosed) {
		cout << "Please select function:";
		cin >> choice;
		if (choice == '2') {
			AdminMenu();
		}
		else if (choice == '1') {
			break;
		}
		else {
			cout << "Wrong Input.. Try Again..";
		}
	}

	cout << "Please Fill in the details. The password must be at least 12 character long\n";


	cout << "Enter Username: ";
	cin >> username;
	string get_user = " select `username`,user_deleted from `user` where `username`= '" + username + "'; ";
	const char* gid = get_user.c_str();
	qstate = mysql_query(conn, gid);

	if (!qstate)
	{
		res = mysql_store_result(conn);

		if (mysql_num_rows(res) == 0) { //check user exist or not
			user_existed = false;
		}
		else {
			user_existed = true;

			while (row = mysql_fetch_row(res)) //get user delete row to use to check whether user is deleted or not
			{
				user_deleted = row[1];
			}
		}
	}
	else
	{
		cout << "Query Execution Problem!" << mysql_error(conn) << endl;
	}
	if (!user_existed) { //if user is not exist
		while (!is_pass_len || !is_pass_correct) {
			cout << "\nEnter Password: ";
			cin >> password;
			if (password.length() < 14) { //if the password length is not correct
				cout << "\nThe password format is wrong!! The password must be at least 12 character long\n";

			}
			else {
				is_pass_len = true;
				cout << "\nEnter the password again: ";
				cin >> password2;
				if (password == password2) {
					is_pass_correct = true;
				}
				else {
					cout << "The password is incorrect please try again.\n";

				}
			}
		}


		string insert_user = "INSERT INTO `user`(`username`, `password`,`role`) VALUES ('" + username + "',SHA1('" + password + "'),2)";
		const char* q = insert_user.c_str();
		qstate = mysql_query(conn, q);
		if (!qstate) {
			cout << "The user is added. \n\n";
			do
			{
				cout << "Do you want to continue this function? Y/N :";
				cin >> choice;
				if (choice == 'y' || choice == 'Y')
				{
					isChoosed = true;
					AddUser();
				}
				else if (choice == 'n' || choice == 'N')
				{
					isChoosed = true;
					ManageUser();
				}
				else {
					cout << "Wrong input! Try Again.";
				}

			} while (!isChoosed);

		}

		else {

			cout << "Query Execution Problem!" << mysql_error(conn) << endl;
		}


	}
	else if (user_existed && user_deleted) { //if user is existed and is deleted
		while (!is_pass_len || !is_pass_correct) {
			cout << "\nEnter Password: ";
			cin >> password;
			if (password.length() < 14) {
				cout << "\nThe password format is wrong!! The password must be at least 12 character long\n";

			}
			else {
				is_pass_len = true;
				cout << "\nEnter the password again: ";
				cin >> password2;
				if (password == password2) {
					is_pass_correct = true;
				}
				else {
					cout << "The password is incorrect please try again.\n";

				}
			}
		}


		string insert_user = "UPDATE `user` SET password = '" + password + "',user_deleted =0 where username= '" + username + "'";
		const char* q = insert_user.c_str();
		qstate = mysql_query(conn, q);
		if (!qstate) {
			cout << "The user is added. \n\n";
			do
			{
				cout << "Do you want to continue this function? Y/N :";
				cin >> choice;
				if (choice == 'y' || choice == 'Y')
				{
					isChoosed = true;
					AddUser();
				}
				else if (choice == 'n' || choice == 'N')
				{
					isChoosed = true;
					ManageUser();
				}
				else {
					cout << "Wrong input! Try Again.";
				}

			} while (!isChoosed);

		}

		else {

			cout << "Query Execution Problem!" << mysql_error(conn) << endl;
		}

	}
	else { //user is existed and not being deleted
		cout << "User is existed!!!";
		do
		{
			cout << "Do you want to continue this function? Y/N :";
			cin >> choice;
			if (choice == 'y' || choice == 'Y')
			{
				isChoosed = true;
				AddUser();
			}
			else if (choice == 'n' || choice == 'N')
			{
				isChoosed = true;
				ManageUser();
			}
			else {
				cout << "Wrong input! Try Again.";
			}

		} while (!isChoosed);
	}
}

void ResetPassword()
{
	char choice;
	string  user_id, username, password, password2;
	bool isChoosed = false, is_pass_len = false, is_pass_correct = false;
	system("cls");
	cout << "Reset User Password\n\n";
	cout << "Please select the user\n\n";

	qstate = mysql_query(conn, "SELECT `userId`, `username` FROM `user` WHERE role = 2 AND user_deleted = 0");

	if (!qstate) {
		cout << setw(11) << "ID" << setw(17) << "User Name\n";
		res = mysql_store_result(conn);
		while (row = mysql_fetch_row(res))
		{
			cout << setw(11) << row[0] << setw(17) << row[1] << endl;
		}
	}
	else {

		cout << "Query Execution Problem!" << mysql_error(conn) << endl;
	}
	cout << "Enter Item ID:";
	cin >> user_id;
	string get_user = "SELECT  `username` FROM `user` WHERE userId = '" + user_id + "' ";
	const char* q_get_user = get_user.c_str();
	qstate = mysql_query(conn, q_get_user);
	if (!qstate) {
		res = mysql_store_result(conn);
		while (row = mysql_fetch_row(res)) {
			username = row[0];
		}
	}
	else {

		cout << "Query Execution Problem!" << mysql_error(conn) << endl;
	}
	cout << "\nUser Selected: " << username << endl;
	cout << "\nThe password must be at least 12 character long\n";

	while (!is_pass_len || !is_pass_correct) {
		cout << "\nEnter New Password: ";
		cin >> password;
		if (password.length() < 14) {
			cout << "\nThe password format is wrong!! The password must be at least 12 character long\n";

		}
		else {
			is_pass_len = true;
			cout << "\nEnter the password again: ";
			cin >> password2;
			if (password == password2) {
				is_pass_correct = true;
			}
			else {
				cout << "The password is incorrect please try again.\n";

			}
		}
	}
	string update_password = "UPDATE `user` SET `password`='" + password + "' WHERE userId='" + user_id + "'";
	const char* q_update_password = update_password.c_str();
	qstate = mysql_query(conn, q_update_password);
	if (!qstate) {
		cout << "The password is reset. \n\n";
		do
		{
			cout << "Do you want to continue this function? Y/N :";
			cin >> choice;
			if (choice == 'y' || choice == 'Y')
			{
				isChoosed = true;
				ResetPassword();
			}
			else if (choice == 'n' || choice == 'N')
			{
				isChoosed = true;
				ManageUser();
			}
			else {
				cout << "Wrong input! Try Again.";
			}

		} while (!isChoosed);
	}
}
 
void DeleteUser()
{
	string  user_id, username;
	char choice;
	bool isChoosed = false;
	system("cls");
	cout << "Delete User\n\n";
	cout << "Please select the user\n\n";
	qstate = mysql_query(conn, "SELECT `userId`, `username` FROM `user` WHERE role = 2 AND user_deleted = 0");

	if (!qstate) {
		cout << setw(11) << "ID" << setw(17) << "User Name\n";
		res = mysql_store_result(conn);
		while (row = mysql_fetch_row(res))
		{
			cout << setw(11) << row[0] << setw(17) << row[1] << endl;
		}
	}
	else {

		cout << "Query Execution Problem!" << mysql_error(conn) << endl;
	}

	cout << "Enter Item ID:";
	cin >> user_id;
	string get_user = "SELECT  `username` FROM `user` WHERE userId = '" + user_id + "' ";
	const char* q_get_user = get_user.c_str();
	qstate = mysql_query(conn, q_get_user);
	if (!qstate) {

		res = mysql_store_result(conn);
		while (row = mysql_fetch_row(res)) {
			username = row[0];
		}
		if (mysql_num_rows(res) != 0 && username != "admin") {

			cout << "\nUser Selected: " << username << endl;
			do
			{
				cout << "Do you want to delete the user? Y/N :";
				cin >> choice;
				if (choice == 'y' || choice == 'Y')
				{
					isChoosed = true;
					string delete_user = "UPDATE `user` SET user_deleted=1 where userId = '" + user_id + "'";
					const char* delete_user_q = delete_user.c_str();
					qstate = mysql_query(conn, delete_user_q);
					if (!qstate)
					{

						cout << "\nuser is deleted\n";
					}
					else {

						cout << "Query Execution Problem!" << mysql_error(conn) << endl;
					}

				}
				else if (choice == 'n' || choice == 'N')
				{
					isChoosed = true;
					ManageUser();
				}
				else {
					cout << "Wrong input! Try Again.";
				}


			} while (!isChoosed);
		}

		else {
			cout << "User does not existed!!!\n";


		}
	}
	else {

		cout << "Query Execution Problem!" << mysql_error(conn) << endl;
	}

	isChoosed = 0;
	do
	{
		cout << "Do you want to continue this function? Y/N :";
		cin >> choice;
		if (choice == 'y' || choice == 'Y')
		{
			isChoosed = 1;
			DeleteUser();
		}
		else if (choice == 'n' || choice == 'N')
		{
			isChoosed = 1;
			ManageUser();
		}
		else {
			cout << "Wrong input! Try Again.\n";
		}

	} while (isChoosed == 0);

}

void RecordRevenue()
{
	int choice, isChoosed = 0;
	system("cls");
	cout << "Record Revenue\n";
	cout << "Select Function\n\n";
	cout << "1. View Previous Revenue\n";
	cout << "2. Calculate Revenue\n";
	cout << "3. Quit\n";

	while (isChoosed == 0) {
		cout << "Please Enter 1-3: ";

		cin >> choice;
		if (!cin) {
			cout << "Invalid format!!! Try Again!!!\n";

			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		else {
			switch (choice)
			{
			case 1:
				ViewRevenue();
				isChoosed = 1;
				break;

			case 2:
				CalculateRevenue();
				isChoosed = 1;
				break;

			case 3:
				AdminMenu();
				isChoosed = 1;
				break;

			default:
				cout << "You entered incorrect number.\n Please Try Again\n";
			}
		}
	}
}
void ViewRevenue()
{
	char choice, isChoosed = 0;
	system("cls");
	cout << "View Previous Revenue\n\n";
	cout << "Revenue Data\n\n";
	qstate = mysql_query(conn, "SELECT revenue_date , revenue FROM `revenue`;   ");

	if (!qstate)
	{
		cout <<string(60,'-') << "\n";
		cout << "|" << setw(17) << "Date" << setw(21) << "|" << "Revenue" << setw(15) << "|" << endl;
		cout << string(60, '-') << "\n";
		res = mysql_store_result(conn);
		while (row = mysql_fetch_row(res))
		{
			cout <<"|" << setw(17) << row[0] << setw(20) << "|" << "RM " << row[1] << setw(15) << "|" << endl;
		}
		cout << string(60, '-') << "\n";

	}
	else {
		cout << "Query Execution Problem!" << mysql_error(conn) << endl;
	}


	cout << "\n\nPress Enter to go back... ";
	_getch();
	RecordRevenue();
}

void CalculateRevenue()
{
	struct item {
		int itemId;
		double item_price;
	};
	bool correct_format = false;
	int month, year;
	double income, total_cost, rent_cost, utility_cost, staff_salary, other_cost, inventory_cost=0, revenue;
	system("cls");
	cout << "Calculate Revenue\n\n";
	while (!correct_format) {
		cout << "Enter the month(example January type 1, February type 2):";
		cin >> month;
		if (!cin) {
			cout << "Invalid format!!! Try Again!!!\n";

			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		else if (month > 12 || month <= 0) {
			cout << "Wrong month format!!! Try Again!!!!\n";
		}
		else {
			correct_format = true;
		}
	}correct_format = false;
	while (!correct_format) {
		
		cout << "\nEnter the year(example: 2022):";
		cin >> year;
		if (!cin) {
			cout << "Invalid format!!! Try Again!!!\n";

			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		else {
			break;
		}
	}correct_format = false;
	while (!correct_format) {
		
	
		cout << "\nEnter the income for that month :RM ";
		cin >> income;
		if (!cin) {
			cout << "Invalid format!!! Try Again!!!\n";

			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		else {
			break;
		}
	}correct_format = false;
	while (!correct_format) {
		
		cout << "\nEnter the rent for that month :RM ";
		cin >> rent_cost;
		if (!cin) {
			cout << "Invalid format!!! Try Again!!!\n";

			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		else {
			break;
		}
	}correct_format = false;
	while (!correct_format) {
		
		cout << "\nEnter the utility for that month :RM ";
		cin >> utility_cost;
		if (!cin) {
			cout << "Invalid format!!! Try Again!!!\n";

			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		else {
			break;
		}
	}correct_format = false;
	while (!correct_format) {
		
		cout << "\nEnter the staff salary for that month :RM ";
		cin >> staff_salary;
		if (!cin) {
			cout << "Invalid format!!! Try Again!!!\n";

			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		else {
			break;
		}
	}correct_format = false;
	while (!correct_format) {
		
		cout << "\nEnter the other expenses (if you does not have other expenses enter 0) :RM ";
		cin >> other_cost;
		if (!cin) {
			cout << "Invalid format!!! Try Again!!!\n";

			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		else {
			break;
		}
	}
	//get item data
	qstate = mysql_query(conn, "select itemId ,price from item where item_delete=0;   ");

	if (!qstate)
	{
		res = mysql_store_result(conn);
		int num_item = 0, x = 0;
		num_item = mysql_num_rows(res);
		item* totalItem;
		totalItem = new item[num_item];
		while (row = mysql_fetch_row(res))
		{
			totalItem[x].itemId = stoi(row[0]);
			totalItem[x].item_price = stod(row[1]);
			x++;
		}
		
		string get_item_using_date = "SELECT itemId,quantity FROM `item_quantity` WHERE month(date) = '" + to_string(month) + "' AND year(date)='" + to_string(year) + "' ";
		const char* gi = get_item_using_date.c_str();
		qstate = mysql_query(conn, gi);

		if (!qstate)
		{
			res = mysql_store_result(conn);
			string itemId;
			while (row = mysql_fetch_row(res))
			{
				
				itemId = row[0];
				for (int i = 0; i < num_item; i++) {
					
					if (to_string(totalItem[i].itemId) == itemId) {
						

						inventory_cost = inventory_cost + (totalItem[i].item_price*stoi(row[1]));
						
					}
				}

			}
		}
		else
		{
			cout << "get_date Query Execution Problem!" << mysql_error(conn) << endl;
		}
		total_cost = rent_cost + utility_cost + staff_salary + other_cost + inventory_cost;
		revenue = income - total_cost;
		cout << "The total revenue is :" << revenue << endl;
		int last_date = GetLastDate(year, month);
		string date = to_string(year) + "-" + to_string(month) + "-" + to_string(last_date);
	
		string add_revenue = "INSERT INTO `revenue`(`revenue_date`, `revenue`, `userId`) VALUES ('" + date + "','" + to_string(revenue) + "','" + userId + "')";
		const char* insert_revenue = add_revenue.c_str();
		qstate = mysql_query(conn, insert_revenue);
		if (!qstate) {
			cout << "The revenue is added to database.\n";
		}

		else
		{
			cout << "add_revenue Query Execution Problem!" << mysql_error(conn) << endl;
		}

	}
	else
	{
		cout << "Query Execution Problem!" << mysql_error(conn) << endl;
	}
	cout << "\n\nPress Enter to go back... ";
	_getch();
	RecordRevenue();
}
//The GenerateReport function to generate the pdf need to install the library for making the pdf. The libraray is called libharu. 
//This program is using vcpkg tools to install the libharu library
void GenerateReport()
{
	int month,choice=0,current_month,current_year,i=0;
	int* previous_months= nullptr;
	int* previous_year= nullptr;
	double* revenue= nullptr;
	//code for get the current date and time
	struct tm newtime;
	time_t now = time(0); // get current date and time
	localtime_s(&newtime, &now);
	int year = 1900 + newtime.tm_year;
	int get_month = 1 + newtime.tm_mon;
	int day = newtime.tm_mday;
	int hour = newtime.tm_hour;
	int minute = 1 + newtime.tm_min;
	int sec = 1 + newtime.tm_sec;
	string date = to_string(year) + to_string(get_month)  + to_string(day) + to_string(hour)  + to_string(minute)  + to_string(sec) ;

	system("cls");
	cout << "Generate Sales Report\n\n";
	cout << "Please Choose the timeframe:\n";
	cout << "1. 3 Month\n";
	cout << "2. 6 Month\n";
	cout << "3. 12 Month\n";
	
	while (choice >= 4 || choice <= 0) { //loop when the user enter is not enter correct choice
		cout << "Please Enter 1-3:";
		cin >> choice;
		if (!cin) {
			cout << "Invalid format!!! Try Again!!!\n";

			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		else {


			if (choice >= 4 || choice <= 0) {
				cout << "Wrong Input Try Again....\n";

			}
		}
		
	}
	system("cls");
	
	if (choice==1) {
		month = 2;
		previous_months = new int[month];
		previous_year = new int[month];
		revenue = new double[month];
		
		getCurrentAndPreviousMonths(current_year, current_month, previous_months, previous_year, month);

		string get_revenue_using_date = "SELECT revenue from revenue WHERE (month(revenue_date) = '" + to_string(current_month) + "' AND year(revenue_date)='" + to_string(current_year) + "') OR (month(revenue_date)='" + to_string(previous_months[0]) + "' AND year(revenue_date)= '" + to_string(previous_year[0]) + "')OR (month(revenue_date)='" + to_string(previous_months[1]) + "' AND year(revenue_date)= '" + to_string(previous_year[1]) + "')";
		const char* gr = get_revenue_using_date.c_str();
		qstate = mysql_query(conn, gr);

		if (!qstate)
		{
			res = mysql_store_result(conn);
			while (row = mysql_fetch_row(res))
			{
				revenue[i] = stod(row[0]);
				i++;
			}
		}
		else
		{
			cout << "Query Execution Problem!" << mysql_error(conn) << endl;
		}
	}
	
	else if (choice == 2) {
		month = 5;
		previous_months = new int[month];
		previous_year = new int[month];
		revenue = new double[month];
		getCurrentAndPreviousMonths(current_year, current_month, previous_months, previous_year, month);

		string get_revenue_using_date = "SELECT revenue from revenue WHERE (month(revenue_date) = '" + to_string(current_month) + "' AND year(revenue_date)='" + to_string(current_year) + "') OR (month(revenue_date)='" + to_string(previous_months[0]) + "' AND year(revenue_date)= '" + to_string(previous_year[0]) + "')OR (month(revenue_date)='" + to_string(previous_months[1]) + "' AND year(revenue_date)= '" + to_string(previous_year[1]) + "')OR (month(revenue_date)='" + to_string(previous_months[2]) + "' AND year(revenue_date)= '" + to_string(previous_year[2]) + "')OR (month(revenue_date)='" + to_string(previous_months[3]) + "' AND year(revenue_date)= '" + to_string(previous_year[3]) + "')OR (month(revenue_date)='" + to_string(previous_months[4]) + "' AND year(revenue_date)= '" + to_string(previous_year[4]) + "')OR (month(revenue_date)='" + to_string(previous_months[5]) + "' AND year(revenue_date)= '" + to_string(previous_year[5]) + "')";
		const char* gr = get_revenue_using_date.c_str();
		qstate = mysql_query(conn, gr);

		if (!qstate)
		{
			res = mysql_store_result(conn);
			while (row = mysql_fetch_row(res))
			{
				revenue[i] = stod(row[0]);
				i++;
			}
		}
		else
		{
			cout << "Query Execution Problem!" << mysql_error(conn) << endl;
		}
	}
	else if (choice == 3) {
		month = 11;
		previous_months = new int[month];
		previous_year = new int[month];
		revenue = new double[month];
		
		getCurrentAndPreviousMonths(current_year, current_month, previous_months, previous_year, month);
		
		string get_revenue_using_date = "SELECT revenue from revenue WHERE (month(revenue_date) = '" + to_string(current_month) + "' AND year(revenue_date)='" + to_string(current_year) + "') OR (month(revenue_date)='" + to_string(previous_months[0]) + "' AND year(revenue_date)= '" + to_string(previous_year[0]) + "')OR (month(revenue_date)='" + to_string(previous_months[1]) + "' AND year(revenue_date)= '" + to_string(previous_year[1]) + "')OR (month(revenue_date)='" + to_string(previous_months[2]) + "' AND year(revenue_date)= '" + to_string(previous_year[2]) + "')OR (month(revenue_date)='" + to_string(previous_months[3]) + "' AND year(revenue_date)= '" + to_string(previous_year[3]) + "')OR (month(revenue_date)='" + to_string(previous_months[4]) + "' AND year(revenue_date)= '" + to_string(previous_year[4]) + "')OR (month(revenue_date)='" + to_string(previous_months[5]) + "' AND year(revenue_date)= '" + to_string(previous_year[5]) + "')OR (month(revenue_date)='" + to_string(previous_months[6]) + "' AND year(revenue_date)= '" + to_string(previous_year[6]) + "')"
			"OR (month(revenue_date)='" + to_string(previous_months[7]) + "' AND year(revenue_date)= '" + to_string(previous_year[7]) + "')OR (month(revenue_date)='" + to_string(previous_months[8]) + "' AND year(revenue_date)= '" + to_string(previous_year[8]) + "')OR (month(revenue_date)='" + to_string(previous_months[9]) + "' AND year(revenue_date)= '" + to_string(previous_year[9]) + "')OR (month(revenue_date)='" + to_string(previous_months[10]) + "' AND year(revenue_date)= '" + to_string(previous_year[10]) + "')OR (month(revenue_date)='" + to_string(previous_months[11]) + "' AND year(revenue_date)= '" + to_string(previous_year[11]) + "')";
		const char* gr = get_revenue_using_date.c_str();
		qstate = mysql_query(conn, gr);

		if (!qstate)
		{
			res = mysql_store_result(conn);
			while (row = mysql_fetch_row(res))
			{
				revenue[i] = stod(row[0]);
				i++;
			}
		}
		else
		{
			cout << "Query Execution Problem!" << mysql_error(conn) << endl;
		}
	}
	double total_revenue=0;
	int* total_month = new int[month + 1];
	int* total_year = new int[month + 1];
	//add the current month and previous month
	
	total_month[0] = current_month;
	total_year[0] = current_year;
	for (int a = 1; a < month+1; a++) {
		total_month[a] = previous_months[a-1];
		total_year[a] = previous_year[a-1];
	
	}


	for (int a = 0; a < i;a++) {
		total_revenue = total_revenue + revenue[a];
	}

	//find the largest revenue
	double largest_revenue = revenue[0];
	for (int a = 1; a < i; a++) {
		if (revenue[a] > largest_revenue) {
			largest_revenue = revenue[a];
		}
	}

	cout << "You Choose the " << month+1 << " month timeframe";
	cout << "\n  ============================================";
	cout << "\n | REPORT FOR " << month+1 << " REVENUE                       |";
	cout << "\n |--------------------------------------------|";
	for (int a = 0; a < i;a++) {
		cout << "\n | Revenue "<< setfill('0') << setw(2)<<total_month[a]<<"/"<< total_year[a]<<"       | \tRM " << revenue[a] << " | ";
	}
	cout << "\n |--------------------------------------------|";
	cout << "\n |  GRAPH FOR "<<month+1 <<" REVENUE                    |";
	cout << "\n |--------------------------------------------|\n";
	

	for (int a = 0; a < month+1; a++)
	{
		cout <<" |" << setfill('0') << setw(2) << total_month[a] << "/" << total_year[a];
	
		cout << "|";

		
		int MAX_BAR_SIZE = 15;
		int barSize = revenue[a] / largest_revenue * MAX_BAR_SIZE;
		for (int j = 0; j < barSize ; j++)
		{
			
			setconsolecolor(0, 2 + a);
			cout << "   ";

		}
		setconsolecolor(7, 0);
		cout << " RM" << revenue[a];
		cout << "\n" ;
	}

	cout << " |---------------------------------------------| ";
	
	cout << "\n |     PERCENTAGE DIFFERENT BETWEEN MONTH     | ";
	cout << "\n |--------------------------------------------|\n";
	//b-1 is current month, b is previous month  
	for (int a = 0, b = i - 1; a < month;a++,b--) { //calculate the revenue percentage different and print it 
		
		double percentage_different=0;
		percentage_different = (revenue[b-1] - revenue[b]) / revenue[b] * 100;
		cout << " |  " << setfill('0') << setw(2) << total_month[b] << "/" << total_year[b] << "-" << setw(2) << total_month[b - 1] << "/" << total_year[b - 1] << " :" << percentage_different << "%\n";
		
		
	}
	cout << " |--------------------------------------------|";
	cout << "\n | Total revenue for "<<month+1<<" month   RM " << total_revenue << "      |";
	cout << "\n |--------------------------------------------|";

	cout << "\n  ============================================" << endl;



	//code for create a pdf
	HPDF_Doc pdf;
	HPDF_Font font1;//bold font
	HPDF_Font font2;//not bold font
	HPDF_Page page_1;
	pdf = HPDF_New(error_handler, NULL);
	font1 = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
	page_1 = HPDF_AddPage(pdf);
	HPDF_Page_SetSize(page_1, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
	HPDF_Page_SetFontAndSize(page_1, font1, 20);
	HPDF_Page_BeginText(page_1);
	string title = "REPORT FOR RESTAURANT";
	HPDF_Page_TextOut(page_1, 175, 800, title.c_str());
	font2 = HPDF_GetFont(pdf, "Helvetica", NULL);
	HPDF_Page_SetFontAndSize(page_1, font2, 14);
	string text1 ="Timeframe : "+to_string(month+1)+" Months";
	HPDF_Page_TextOut(page_1, 30, 750, text1.c_str());
	string text2 = "REVENUE FOR EACH MONTH:";
	HPDF_Page_TextOut(page_1, 30, 700, text2.c_str());
	int b = 675; //b is the y axis 
	for (int a = 0; a < i; a++,b-=25) { //print the revenue for each month
		
		string text3 = "    Revenue " + to_string(total_month[a]) + "/" + to_string(total_year[a]) + "       | \tRM " + to_string(revenue[a]) + " | ";
		HPDF_Page_TextOut(page_1, 30, b, text3.c_str());
		
	}
	b -= 20;
	string graph_text = "GRAPH FOR "+ to_string(month + 1) + " MONTH REVENUE  ";
	HPDF_Page_TextOut(page_1, 30, b, graph_text.c_str());
	b -= 30;
	string header_text = " DATE ";
	HPDF_Page_TextOut(page_1, 30, b, header_text.c_str());
	string header_text2 = "REVENUE ";
	HPDF_Page_TextOut(page_1, 200, b, header_text2.c_str());
	b -= 10;
	string line = "_____________________________________________________________ ";
	HPDF_Page_TextOut(page_1, 30, b, line.c_str());
	b -= 30;
	HPDF_Page_EndText(page_1);
	
	//color
	float r[12] = {0.9,0.9,0.0,0.6,0.0,0.4,0.8,0.3,0.0,0.6,0.9,0.9};
	float g[12] = { 0.0,0.6,0.9,0.9,0.9,0.9,0.9,0.6,0.0,0.0,0.0,0.0};
	float blue[12] = { 0.0,0.0,0.6,0.0,0.0,0.6,0.6,0.9,0.9,0.9,0.9,0.6 };
	//code for making graph
	for (int a = 0; a < i; a++, b -= 25) {
		HPDF_Page_SetRGBFill(page_1, r[a], g[a], blue[a]);
		HPDF_Page_BeginText(page_1);
		string text4 =" |"+ to_string(total_month[a]) + "/" + to_string(total_year[a])+ "|";
		HPDF_Page_TextOut(page_1, 30, b, text4.c_str());
		HPDF_Page_EndText(page_1);
		
		int MAX_BAR_SIZE = 250;
		int barSize = (double)revenue[a] / largest_revenue * MAX_BAR_SIZE;
		HPDF_Page_Rectangle(page_1, 200, b, barSize, 10);
		HPDF_Page_FillStroke(page_1);
		HPDF_Page_BeginText(page_1);
		string revenue_text = "RM "+to_string(revenue[a]);
		HPDF_Page_TextOut(page_1, MAX_BAR_SIZE +220, b, revenue_text.c_str());
		HPDF_Page_EndText(page_1);
	}
	HPDF_Page_BeginText(page_1);
	HPDF_Page_SetRGBFill(page_1, 0.0, 0.0, 0.0);
	b -= 35;
	if (month+1<=6) {
		string text5 = " PERCENTAGE DIFFERENT BETWEEN MONTH  ";
		HPDF_Page_TextOut(page_1, 30, b, text5.c_str());

		b -= 35;
		for (int a = 0, c = i - 1; a < month; a++, c--, b -= 25) { //print the precentage different

			double percentage_different = 0;
			percentage_different = (revenue[c - 1] - revenue[c]) / revenue[c] * 100;
			string text6 = " |  " + to_string(total_month[c]) + "/" + to_string(total_year[c]) + "-" + to_string(total_month[c - 1]) + "/" + to_string(total_year[c - 1]) + " :" + to_string(percentage_different) + "%\n";
			HPDF_Page_TextOut(page_1, 30, b, text6.c_str());

		}
		b -= 35;
		string total_revenue_text = " TOTAL REVENUE FOR " + to_string(month + 1) + " MONTH   RM " + to_string(total_revenue);
		HPDF_Page_TextOut(page_1, 30, b, total_revenue_text.c_str());
		HPDF_Page_EndText(page_1);
	}
	else {//if the month is 12 
		HPDF_Page_EndText(page_1);
		HPDF_Page page_2;
		
		page_2 = HPDF_AddPage(pdf);
		HPDF_Page_SetSize(page_2, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
		HPDF_Page_SetFontAndSize(page_2, font2, 14);
		HPDF_Page_BeginText(page_2);
		 b = 800;
		string text5 = " PERCENTAGE DIFFERENT BETWEEN MONTH  ";
		HPDF_Page_TextOut(page_2, 30, b, text5.c_str());

		b -= 35;
		for (int a = 0, c = i - 1; a < month; a++, c--, b -= 25) { //print the precentage different

			double percentage_different = 0;
			percentage_different = (revenue[c - 1] - revenue[c]) / revenue[c] * 100;
			string text6 = " |  " + to_string(total_month[c]) + "/" + to_string(total_year[c]) + "-" + to_string(total_month[c - 1]) + "/" + to_string(total_year[c - 1]) + " :" + to_string(percentage_different) + "%\n";
			HPDF_Page_TextOut(page_2, 30, b, text6.c_str());

		}
		b -= 35;
		string total_revenue_text = " TOTAL REVENUE FOR " + to_string(month + 1) + " MONTH   RM " + to_string(total_revenue);
		HPDF_Page_TextOut(page_2, 30, b, total_revenue_text.c_str());
		HPDF_Page_EndText(page_2);
	
	
	}
	string file_name = "REPORT" + date+".pdf";
	HPDF_SaveToFile(pdf, file_name.c_str());
	HPDF_Free(pdf);
	//code for open the pdf 
	string dir = fs::current_path().string()+"\\REPORT"+date+".pdf";
	ShellExecuteA(NULL, "open", dir.c_str(), 0, 0, SW_SHOW);
	cout << "\n The pdf is generated...\nOpening the pdf file....";
	cout << "\n\nPress Enter to go back... ";
	_getch();
	AdminMenu();
}

//staff's function

void StaffMenu() {
	int choice, isChoosed = 0;

	system("cls");
	cout << "Staff Menu\n";
	cout << "Select Module\n";
	cout << "1. Manage Inventory Module\n2. Reset Password\n3. Quit\n";



	while (isChoosed == 0) {
		cout << "Please Enter 1-2: ";
		
			cin >> choice;
			if (!cin) {
				cout << "Invalid format!!! Try Again!!!\n";

				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}
			else {

				switch (choice)
				{
				case 1:
					isChoosed = 1;
					ManageInventory();
					break;

				case 2:
					isChoosed = 1;
					ResetPasswordStaff();
					break;

				case 3:
					isChoosed = 1;
					Quit();
					break;

				default:
					cout << "You entered incorrect number.\n Please Try Again \n";
					break;
				}
			}
		}


}

void ManageInventory()
{
	int choice, isChoosed = 0;
	system("cls");
	cout << "Manage Inventory Module\n";
	cout << "Select Function\n";
	cout << "1. View Inventory/Update Item Quantity\n2. Add Item\n3. Delete Item\n4. Quit\n";

	while (isChoosed == 0) {
		cout << "Please Enter 1-4: ";
		cin >> choice;
		//error handling prevent the user enter wrong format data
		if (!cin) {
			cout << "Invalid format!!! Try Again!!!\n";

			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		else {
			switch (choice)
			{
			case 1:
				isChoosed = 1;
				ViewInventory();
				break;

			case 2:
				isChoosed = 1;
				AddItem();
				break;

			case 3:
				isChoosed = 1;
				DeleteItem();
				break;

			case 4:
				isChoosed = 1;
				StaffMenu();
				break;

			default:
				cout << "You entered incorrect number.\n Please Try Again \n";
				break;
			}
		}
	}
}

void ViewInventory()
{
	string item_id, item_name;
	bool minimum_quantity_alert = false;
	int choice, isChoosed = 0;


	system("cls");
	cout << "View Inventory\n\n";
	cout << "Inventory\n";
	cout << "";
	qstate = mysql_query(conn, "select itemId , item_name,quantity ,min_quantity, price from item where item_delete=0;   ");

	if (!qstate)
	{

		cout << setw(11) << "ID" << setw(17) << "Item Name" << setw(20) << "Quantity" << setw(20) << "Price" << setw(25) << "Minimum Quantity " << endl;

		res = mysql_store_result(conn);
		while (row = mysql_fetch_row(res))
		{
			cout << setw(11) << row[0] << setw(17) << row[1] << setw(20) << row[2] << setw(20) << "RM " << row[4] << setw(15) << row[3] << endl;
			if (stoi(row[2]) < stoi(row[3])) { //if the quantity is lower than the item mininum quantity
				//add the item id and name into the item_id and item_name
				item_id.append(row[0]); item_id.append("; ");
				item_name.append(row[1]); item_name.append("; ");
				minimum_quantity_alert = true;
			}
		}
		if (minimum_quantity_alert) {
			cout << "ID: " << item_id << "Item Name: " << item_name << "is below the minimum quantity. Please restock.\n";

		}


	}
	else
	{
		cout << "Query Execution Problem!" << mysql_error(conn) << endl;
	}
	cout << "\nSelect:\n1. View Item Quantity History\n2. Update Item Quantity\n3. Quit\n";
	while (isChoosed == 0) {//loop if the user enter incorrect choice
		cout << "Please Enter 1-3: ";
		cin >> choice;
		if (!cin) { //if the user is entering wrong data type(for example string )
			cout << "Invalid format!!! Try Again!!!\n";

			cin.clear();//it will clear the input error
			cin.ignore(numeric_limits<streamsize>::max(), '\n'); // it will skip all the input 
		}
		else {


			switch (choice)
			{
			case 1:
				isChoosed = 1;
				ViewItemHistory();
				break;

			case 2:
				isChoosed = 1;
				UpdateQuantity();
				break;

			case 3:
				isChoosed = 1;
				ManageInventory();
				break;

			default:
				cout << "You entered incorrect number.\n Please Try Again \n";
				break;
			}
		}
	}

}

void ViewItemHistory()
{
	string  item_id, item_name, item_min_qty;
	bool minimum_quantity_alert = false, item_deleted = false;
	int i = 0;

	cout << "Enter Item ID:";
	cin >> item_id;
	system("cls");
	string get_item_detail = " select `item_name`, `min_quantity` from `item` where `itemId`= '" + item_id + "' AND item_delete = 0; ";
	string get_quantity_query = "select date, quantity from item_quantity where itemId= '" + item_id + "'; ";
	const char* gid = get_item_detail.c_str();
	const char* gqq = get_quantity_query.c_str();

	qstate = mysql_query(conn, gid);

	if (!qstate)
	{
		res = mysql_store_result(conn);

		if (mysql_num_rows(res) == 0) { //check the item is deleted or not exist
			item_deleted = true;
		}
		else {//if the item is not deleted

			while (row = mysql_fetch_row(res))
			{
				item_name = row[0];
				item_min_qty = row[1];

			}
		}


	}
	else
	{
		cout << "Query Execution Problem!" << mysql_error(conn) << endl;
	}

	if (!item_deleted) {//if the item is not deleted 
		qstate = mysql_query(conn, gqq);
		if (!qstate)
		{

			cout << setw(21) << "Date" << setw(17) << "Item Name" << setw(20) << "Quantity" << setw(20) << "Minimum Quantity " << endl;

			res = mysql_store_result(conn);

			while (row = mysql_fetch_row(res))
			{
				cout << setw(20) << row[0] << setw(17) << item_name << setw(20) << row[1] << setw(20) << item_min_qty << endl;
				i++;

				if (stoi(row[1]) < stoi(item_min_qty) && i == mysql_num_rows(res)) { //if the latest quantity is less than minimum quantity

					minimum_quantity_alert = true;

				}


			}


		}
		else
		{
			cout << "Query Execution Problem!\n" << mysql_error(conn) << endl;
		}
	}
	else {//if the item is deleted or not exist
		cout << "Item not found...";


	}

	if (minimum_quantity_alert) {

		cout << "\n" << item_name << " is below the minimum quantity!!!! Please restock!!!!!!\n";

	}

	cout << "\n\nPress Enter to go back... ";

	_getch();
	ViewInventory();


}



void AddItem()
{
	string item_name, item_id;
	double item_price;
	int item_quantity, item_min_quantity;
	char choice,select_function;
	bool isChoosed = false, item_deleted = false, correct_format = false;
	struct tm newtime;
	time_t now = time(0); // get current date and time
	localtime_s(&newtime, &now);
	int year = 1900 + newtime.tm_year;
	int month = 1 + newtime.tm_mon;
	int day = newtime.tm_mday;
	int hour = newtime.tm_hour;
	int minute = 1 + newtime.tm_min;
	int sec = 1 + newtime.tm_sec;
	string date = to_string(year) + "-" + to_string(month) + "-" + to_string(day) + " " + to_string(hour) + ":" + to_string(minute) + ":" + to_string(sec) + ":";


	system("cls");
	cout << "Add Item\n";
	while (!isChoosed) {
		cout << "\n1. Add item \n2. Quit";
		cout << "\nSelect the function :";
		cin >> select_function;
		if (!cin) {
			cout << "Invalid format!!! Try Again!!!\n";

			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		else {
			if (select_function == '2') { //if the user choose quit
				isChoosed = true;
				StaffMenu();

			}
			else if (select_function == '1') {
				isChoosed = true;
				cout << "Please fill up all the detail\n";
				cin.ignore(1, '\n');
				cout << "Item name:";
				getline(cin, item_name);
				string itemId;
				//check item is deleted previously or not
				string get_item_detail = " select `itemId`,item_delete from `item` where `item_name`= '" + item_name + "'; ";
				const char* gid = get_item_detail.c_str();
				qstate = mysql_query(conn, gid);

				if (!qstate)
				{

					res = mysql_store_result(conn);
					while (row = mysql_fetch_row(res))
					{
						itemId = row[0];
						istringstream(row[1]) >> item_deleted; //convert string to bool 
					}
					if (item_deleted) { //if the item is deleted 
						while (!correct_format) {
							cout << "Item price:";
							cin >> item_price;
							if (!cin) { //if the data type is not correct
								cout << "Invalid format!!! Try Again!!!\n";

								cin.clear();
								cin.ignore(numeric_limits<streamsize>::max(), '\n');
							}
							else {
								correct_format = true;
							}
						}correct_format = false;
						while (!correct_format) {
							
							cout << "Item minimum quantity:";
							cin >> item_min_quantity;
							if (!cin) {
								cout << "Invalid format!!! Try Again!!!\n";

								cin.clear();
								cin.ignore(numeric_limits<streamsize>::max(), '\n');
							}
							else {
								correct_format = true;
							}
						}correct_format = false;
						while (!correct_format) {
							
							cout << "Item quantity:";
							if (!cin) {
								cout << "Invalid format!!! Try Again!!!\n";

								cin.clear();
								cin.ignore(numeric_limits<streamsize>::max(), '\n');
							}
							else {
								correct_format = true;
							}
						}correct_format = false;
						while (!correct_format) {
							
							cin >> item_quantity;
							if (!cin) {
								cout << "Invalid format!!! Try Again!!!\n";

								cin.clear();
								cin.ignore(numeric_limits<streamsize>::max(), '\n');
							}
							else {
								correct_format = true;
							}
						}
						string add_item = "UPDATE `item` SET `min_quantity`='" + to_string(item_min_quantity) + "',`price`='" + to_string(item_price) + "',`userId`='" + userId + "',`quantity`='" + to_string(item_quantity) + "',`item_delete`='0' WHERE itemId ='" + itemId + "' ";
						const char* q = add_item.c_str();
						qstate = mysql_query(conn, q);
						if (!qstate)
						{
							string add_quantity = "INSERT INTO `item_quantity`(`itemId`, `quantity`, `date`) VALUES  ('" + itemId + "','" + to_string(item_quantity) + "','" + date + "')";
							const char* insert_quan = add_quantity.c_str();
							qstate = mysql_query(conn, insert_quan);
							if (!qstate) {
								cout << "Item is added. ";

								do
								{
									cout << "Do you want to continue this function? Y/N :";
									cin >> choice;
									if (choice == 'y' || choice == 'Y')
									{
										isChoosed = true;
										AddItem();
									}
									else if (choice == 'n' || choice == 'N')
									{
										isChoosed = true;
										ManageInventory();
									}
									else {
										cout << "Wrong input! Try Again.";
									}

								} while (!isChoosed);

							}
							else
							{
								cout << "Query Execution Problem!\n" << mysql_error(conn) << endl;
							}


						}
						else
						{
							cout << "Query Execution Problem!" << mysql_error(conn) << endl;
						}
					}
					else if (!item_deleted && mysql_num_rows(res) == 0) {//if the item does not exist
						while (!correct_format) {
							cout << "Item price:";
							cin >> item_price;
							if (!cin) {
								cout << "Invalid format!!! Try Again!!!\n";

								cin.clear();
								cin.ignore(numeric_limits<streamsize>::max(), '\n');
							}
							else {
								correct_format = true;
							}
						}correct_format = false;
						while (!correct_format) {
							
							cout << "Item minimum quantity:";
							cin >> item_min_quantity;
							if (!cin) {
								cout << "Invalid format!!! Try Again!!!\n";

								cin.clear();
								cin.ignore(numeric_limits<streamsize>::max(), '\n');
							}
							else {
								correct_format = true;
							}
						}correct_format = false;
						while (!correct_format) {
							
							cout << "Item quantity:";
							cin >> item_quantity;
							if (!cin) {
								cout << "Invalid format!!! Try Again!!!\n";

								cin.clear();
								cin.ignore(numeric_limits<streamsize>::max(), '\n');
							}
							else {
								correct_format = true;
							}
						}

						string add_item = "INSERT INTO `item`(`item_name`, `min_quantity`, `price`, `userId`, `quantity`) VALUES  ('" + item_name + "', '" + to_string(item_min_quantity) + "', '" + to_string(item_price) + "', '" + userId + "', '" + to_string(item_quantity) + "')";
						const char* q = add_item.c_str();
						qstate = mysql_query(conn, q);
						if (!qstate)
						{
							string get_itemID = " select `itemId` from `item` where `item_name`= '" + item_name + "'; ";
							const char* get_id = get_itemID.c_str();
							qstate = mysql_query(conn, get_id);
							if (!qstate)
							{
								res = mysql_store_result(conn);

								while (row = mysql_fetch_row(res))
								{
									item_id = row[0];


								}
								string add_quantity = "INSERT INTO `item_quantity`(`itemId`, `quantity`, `date`) VALUES  ('" + item_id + "','" + to_string(item_quantity) + "','" + date + "')";
								const char* insert_quan = add_quantity.c_str();
								qstate = mysql_query(conn, insert_quan);
								if (!qstate) {
									cout << "Item is added. ";

									do
									{
										isChoosed = false;
										cout << "Do you want to continue this function? Y/N :";
										cin >> choice;
										if (choice == 'y' || choice == 'Y')
										{
											isChoosed = true;
											AddItem();
										}
										else if (choice == 'n' || choice == 'N')
										{
											isChoosed = true;
											ManageInventory();
										}
										else {
											cout << "Wrong input! Try Again.";
										}

									} while (!isChoosed);

								}
								else
								{
									cout << "Query Execution Problem!\n" << mysql_error(conn) << endl;
								}

							}
							else {

								cout << "Query Execution Problem!" << mysql_error(conn) << endl;
							}

						}
						else
						{
							cout << "Query Execution Problem!" << mysql_error(conn) << endl;
						}


					}
					else {//if the item is not deleted

						cout << "Item Already Existed!!!";
						cout << "\n\nPress Enter to go back... ";

						_getch();
						ManageInventory();
					}


				}
				else
				{
					cout << "Query Execution Problem!" << mysql_error(conn) << endl;
				}

			}
			else {
				cout << "Wrong Input.. Try Again..";
			}
		}
	}
}







void UpdateQuantity()
{
	string item_id;
	int item_quantity;
	bool item_deleted = false;
	//code for get the current date and time
	struct tm newtime;
	time_t now = time(0); // get current date and time
	localtime_s(&newtime, &now);
	int year = 1900 + newtime.tm_year;
	int month = 1 + newtime.tm_mon;
	int day = newtime.tm_mday;
	int hour = newtime.tm_hour;
	int minute = 1 + newtime.tm_min;
	int sec = 1 + newtime.tm_sec;
	string date = to_string(year) + "-" + to_string(month) + "-" + to_string(day) + " " + to_string(hour) + ":" + to_string(minute) + ":" + to_string(sec) + ":";


	cout << "\nEnter the item ID: ";
	cin >> item_id;
	//check the item is deleted or not 
	string check_item = "select item_name  from `item` where itemId='" + item_id + "' AND item_delete=0; ";
	const char* ci = check_item.c_str();
	qstate = mysql_query(conn, ci);

	if (!qstate)
	{
		res = mysql_store_result(conn);

		if (mysql_num_rows(res) == 0) {
			item_deleted = true;
		}
	}
	else
	{
		cout << "Query Execution Problem!" << mysql_error(conn) << endl;
	}
	if (!item_deleted) { //if the item is not deleted
		bool correct_format = false;
		while (!correct_format) {
			
			cout << "\nEnter the item quantity: ";
			cin >> item_quantity;
			if (!cin) {
				cout << "Invalid format!!! Try Again!!!\n";

				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}
			else {
				correct_format = true;
			}
		}
		string update_query = "update item set quantity = '" + to_string(item_quantity) + "' where itemId = '" + item_id + "'";
		const char* update = update_query.c_str();
		qstate = mysql_query(conn, update);
		if (!qstate)
		{
			string add_quantity = "INSERT INTO `item_quantity`(`itemId`, `quantity`, `date`) VALUES  ('" + item_id + "','" + to_string(item_quantity) + "','" + date + "')";
			const char* insert_quan = add_quantity.c_str();
			qstate = mysql_query(conn, insert_quan);
			if (!qstate) {

				cout << "Quantity is updated.";
				cout << "\n\nPress Enter to go back... ";

				_getch();
				ViewInventory();

			}
			else
			{
				cout << "Query Execution Problem!\n" << mysql_error(conn) << endl;
			}

		}
		else
		{
			cout << "Query Execution Problem!\n" << mysql_error(conn) << endl;
		}
	}
	else { //if the item is deleted or not exist
		cout << "Item not found...";
		cout << "\n\nPress Enter to go back... ";
	}
	_getch();
	ViewInventory();
}

void DeleteItem()
{
	string item_id;
	char choice1, choice2;
	bool isChoosed = 0;
	bool item_deleted = false;
	
	system("cls");
	cout << "Delete Item\n\n";
	cout << "1. Delete Item\n2. Quit\n";
	while (!isChoosed) {
		cout << "Please select function:";
		cin >> choice2;
		if (choice2=='2') {
			StaffMenu();
		}
		else if (choice2 == '1') {
			break;
		}
		else {
			cout << "Wrong Input.. Try Again..";
		}
	}
	cout << "Please select the item\n\n";
	qstate = mysql_query(conn, "select itemId , item_name from item where item_delete = 0;   ");

	if (!qstate)
	{

		cout << setw(11) << "ID" << setw(17) << "Item Name" << endl;

		res = mysql_store_result(conn);
		while (row = mysql_fetch_row(res))
		{
			cout << setw(11) << row[0] << setw(17) << row[1] << endl;
		}
	}
	else {
		cout << "Query Execution Problem!" << mysql_error(conn) << endl;

	}

	cout << "Enter the item ID: ";
	cin >> item_id;
	string get_item_detail = " select `item_name` from `item` where `itemId`= '" + item_id + "' AND item_delete = 0; ";
	const char* gid = get_item_detail.c_str();
	qstate = mysql_query(conn, gid);

	if (!qstate)
	{
		res = mysql_store_result(conn);

		if (mysql_num_rows(res) == 0) {
			item_deleted = true;
		}
	}
	else
	{
		cout << "Query Execution Problem!" << mysql_error(conn) << endl;
	}
	if (!item_deleted) { //if the item is not deleted
		do {
			cout << "Do you want to delete the item? Y/N :";
			cin >> choice1;
			if (choice1 == 'y' || choice1 == 'Y') {
				isChoosed = 1;

				string delete_query = "UPDATE `item` SET `item_delete`='1' WHERE  itemId = '" + item_id + "'";
				const char* q = delete_query.c_str();
				qstate = mysql_query(conn, q);
				if (!qstate)
				{
					isChoosed = 0;
					string delete_quantity_query = "DELETE FROM `item_quantity`  WHERE  itemId = '" + item_id + "'";
					const char* q = delete_quantity_query.c_str();
					qstate = mysql_query(conn, q);
					if (!qstate)
					{
						cout << "\nItem is deleted\n";
						do
						{
							cout << "Do you want to continue this function? Y/N :";
							cin >> choice2;
							if (choice2 == 'y' || choice2 == 'Y')
							{
								isChoosed = 1;
								DeleteItem();
							}
							else if (choice2 == 'n' || choice2 == 'N')
							{
								isChoosed = 1;
								ManageInventory();
							}
							else {
								cout << "Wrong input! Try Again.\n";
							}

						} while (!isChoosed);
					}
					else {

						cout << "Query Execution Problem!" << mysql_error(conn) << endl;
					}

				}
				else {

					cout << "Query Execution Problem!" << mysql_error(conn) << endl;
				}



			}
			else if (choice1 == 'n' || choice1 == 'N') {
				isChoosed = 1;
				DeleteItem();

			}
			else {
				cout << "Wrong input! Try Again.\n";

			}

		} while (isChoosed == 0);
	}
	else { //if the item is deleted or not exist
		cout << "Item not found...";
		cout << "\n\nPress Enter to go back... ";

		_getch();
		ManageInventory();
	}
}

void ResetPasswordStaff()
{
	char choice;
	string  user_id=userId, username, password, password2;
	bool isChoosed = false, is_pass_len = false, is_pass_correct = false;
	system("cls");
	
	cout << "Reset User Password\n\n";
	cout << "Select the function\n";
	cout << "1. Reset Password\n2. Quit\n";
	while (!isChoosed) {
		cout << "Please select function:";
		cin >> choice;
		if (choice == '1') {
			break;
		}
		else if (choice == '2') {
			StaffMenu();
		}
		else {
			cout << "Wrong Input.. Try Again..";
		}
	}
	cout << "\nThe password must be at least 12 character long\n";

	while (!is_pass_len || !is_pass_correct) {
		cout << "\nEnter New Password: ";
		cin >> password;
		if (password.length() < 14) {
			cout << "\nThe password format is wrong!! The password must be at least 12 character long\n";

		}
		else {
			is_pass_len = true;
			cout << "\nEnter the password again: ";
			cin >> password2;
			if (password == password2) {
				is_pass_correct = true;
			}
			else {
				cout << "The password is incorrect please try again.\n";

			}
		}
	}
	string update_password = "UPDATE `user` SET `password`=SHA1('" + password + "') WHERE userId='" + user_id + "'";
	const char* q_update_password = update_password.c_str();
	qstate = mysql_query(conn, q_update_password);
	if (!qstate) {
		cout << "The The password is reset. \n\n";
		cout << "\n\nPress Enter to go back... ";
		_getch();
		StaffMenu();
	}
}

void Quit()
{
	system("cls");
	cout << "Thank you for using Restaurant Management System";
	exit(0);
}

