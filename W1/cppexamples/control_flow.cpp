# include <iostream>
using namespace std;
//成绩判定
int main (){
    int score;
    cout<<"请输入分数：";
    cin>> score;

    if (score >=90)
    {cout<<"优秀"<< endl;

        /* code */
    }
    else if (score>=60)
    {cout <<"及格"<< endl;
        /* code */
    }else{
        cout<<"不及格"<< endl;
    }
    //判断奇偶数
    int num;
    cout<<"请输入数字：";
    cin>> num;

    if (num % 2 == 0) {
        cout << num << " 是偶数" << endl;
    } else {
        cout << num << " 是奇数" << endl;}
    // 判断闰年
    int year ;
    cout <<"请输入年份:";
    cin>> year;
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        cout << year << " 是闰年" << endl;
    } else {
        cout << year << " 不是闰年" << endl;}
return 0;
    }

