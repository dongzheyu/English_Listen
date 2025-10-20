#include <bits/stdc++.h>
#include <windows.h>
using namespace std;
string message,word;
int i=0;
vector<string> words;
void listen(){
    message="echo CreateObject(\"SAPI.SpVoice\").Speak(\""+words[i]+"\") > %temp%\\temp_speak.vbs && cscript //nologo %temp%\\temp_speak.vbs && del %temp%\\temp_speak.vbs";
}

// 从词库文件中读取所有的单词到 vector 中
vector<string> loadWordsFromFile(const string& filename) {

    ifstream fin(filename);
    
    // 检查文件是否成功打开
    if (!fin.is_open()) {
        cout << "无法打开词库文件: " << filename << endl;
        return words; // 返回空的 vector
    }
    
    string w;
    // 逐行读取文件中的单词
    while (getline(fin, w)) {
        // 忽略空行
        if (!w.empty()) {
            words.push_back(w);
        }
    }
    
    fin.close();
    return words;
}

int main(){
    while (true){
        system("title 英语听考程序");
        system("cls");
        cout<<"==========欢迎使用英语听考程序==========\n";
        cout<<"[1]输入词库\n";
        cout<<"[2]开始听考\n";
        cout<<"请输入选项数字：";
        int choice;
        cin>>choice;
        switch (choice) {
        case 1: {
            system("cls");
            cout<<"请输入要添加的单词（输入end结束）：\n";
            ofstream fout("wordlist.txt",ios::app);
            while(true){
                cin>>word;
                if(word=="end") break;
                fout<<word<<endl;
            }
            fout.close();
            cout<<"词库添加完成！\n";
            system("pause");
            break;
        }
        case 2: {
            system("cls");
            // 读取词库文件中的单词
            vector<string> words = loadWordsFromFile("wordlist.txt");
            if (words.empty()) {
                cout << "词库为空，请先添加单词。" << endl;
                break;
            }
            cout<<"开始听考！\n";
            for (i=0;i<words.size();i++){
                listen();
                system(message.c_str());
                for (int j=5;j>=0;j--){
                    cout<<j<<"秒后进行下一个单词……\n";
                    Sleep(1000);
                    system("cls");
                }
            }
            cout<<"听考结束！\n";
            system("pause");
            break;
        }
        default: {
            cout<<"无效选项，程序结束。\n";
            system("pause");
            break;
        }
        }
    }
}