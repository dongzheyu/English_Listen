#include <bits/stdc++.h>
#include <windows.h>
using namespace std;
string message,word;
int i=0;
vector<string> words;
void listen(){
    message="echo CreateObject(\"SAPI.SpVoice\").Speak(\""+words[i]+"\") > %temp%\\temp_speak.vbs && cscript //nologo %temp%\\temp_speak.vbs && del %temp%\\temp_speak.vbs";
}

// �Ӵʿ��ļ��ж�ȡ���еĵ��ʵ� vector ��
vector<string> loadWordsFromFile(const string& filename) {

    ifstream fin(filename);
    
    // ����ļ��Ƿ�ɹ���
    if (!fin.is_open()) {
        cout << "�޷��򿪴ʿ��ļ�: " << filename << endl;
        return words; // ���ؿյ� vector
    }
    
    string w;
    // ���ж�ȡ�ļ��еĵ���
    while (getline(fin, w)) {
        // ���Կ���
        if (!w.empty()) {
            words.push_back(w);
        }
    }
    
    fin.close();
    return words;
}

int main(){
    while (true){
        system("title Ӣ����������");
        system("cls");
        cout<<"==========��ӭʹ��Ӣ����������==========\n";
        cout<<"[1]����ʿ�\n";
        cout<<"[2]��ʼ����\n";
        cout<<"������ѡ�����֣�";
        int choice;
        cin>>choice;
        switch (choice) {
        case 1: {
            system("cls");
            cout<<"������Ҫ��ӵĵ��ʣ�����end��������\n";
            ofstream fout("wordlist.txt",ios::app);
            while(true){
                cin>>word;
                if(word=="end") break;
                fout<<word<<endl;
            }
            fout.close();
            cout<<"�ʿ������ɣ�\n";
            system("pause");
            break;
        }
        case 2: {
            system("cls");
            // ��ȡ�ʿ��ļ��еĵ���
            vector<string> words = loadWordsFromFile("wordlist.txt");
            if (words.empty()) {
                cout << "�ʿ�Ϊ�գ�������ӵ��ʡ�" << endl;
                break;
            }
            cout<<"��ʼ������\n";
            for (i=0;i<words.size();i++){
                listen();
                system(message.c_str());
                for (int j=5;j>=0;j--){
                    cout<<j<<"��������һ�����ʡ���\n";
                    Sleep(1000);
                    system("cls");
                }
            }
            cout<<"����������\n";
            system("pause");
            break;
        }
        default: {
            cout<<"��Чѡ����������\n";
            system("pause");
            break;
        }
        }
    }
}