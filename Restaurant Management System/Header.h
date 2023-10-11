#pragma once

void Login();
void AdminMenu();
void StaffMenu();
int main();


int qstate;
MYSQL* conn;
MYSQL_ROW row;
MYSQL_RES* res;