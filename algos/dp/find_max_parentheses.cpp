#include <iostream>
using namespace std;

int find_max_parentheses_count(char *, int);

int main()
{
    int case_count, char_num;
    while(cin >> case_count)
    {
        int *rets = new int[case_count];
        for(int j=0;j<case_count; ++j){
            cin >> char_num;
            char *chars = new char[char_num];
            for(int i=0; i< char_num; ++i)
            {
                cin >> chars[i];
            }
            rets[j] = find_max_parentheses_count(chars, char_num);
        }
        
        for(int i=0; i<case_count; ++i)
        {
            cout<<rets[i]<<endl;
        }
        
        delete rets;
    }
}

int find_max_parentheses_count(char *c, int num)
{
    if (num ==0) return 1;
    else if(num == 1) return 0;

    int **dp = new int*[num+1];
    
    for (int i = 0; i<=num; ++i)
    {
        dp[i] = new int[num+1];
        dp[i][i] = 0;
    }    


    for(int i=num-2; i>=0; --i)
    {
        for(int k=i+1;k<num;++k)
        {
            if(c[i]==')')
            {
                dp[i][k] = dp[i+1][k];
            }
            else if(k==i+1)
            {
                if (c[k]==')')
                    dp[i][k] = 1;
                else
                    dp[i][k] = 0;
            }
            else
            {
                int tmp;
                tmp = dp[i+1][k] > dp[i][k-1] ? dp[i+1][k] : dp[i][k-1];
                if(c[k]==')')
                {
                    int max = 0;
                    for(int j=i+1; j<k; ++j)
                    {
                        int tmp3 = dp[i+1][j] * dp[j][k-1];
                        max = max > tmp3 ? max : tmp3;
                    }
                    tmp = tmp > max+1 ? tmp : max+1 ;
                    tmp = tmp > dp[i+1][k-1] + 1 ? tmp : dp[i+1][k-1]+1;
                }
                else
                {
                    int max = 0;
                    for(int j = i+1; j<k; ++j)
                    {
                        int tmp2 = dp[i][j] * (dp[j][k]+1);
                        max = max > tmp2 ? max : tmp2;
                    }
                    tmp = tmp > max? tmp : max;
                }
                
                dp[i][k] = tmp;
            }
        }
    }

    for(int i=0; i<=num; ++i)
    {
        for(int j=0; j<=num; ++j)
        {
            cout << dp[i][j] << "  ";
        }
        cout << endl;
    }

    int ret = dp[0][num-1];
    for(int i=0;i<=num; ++i)
    {
        delete dp[i];
    }
    delete dp;
    return ret;

}
