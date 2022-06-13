
#include "CFG.h"

using namespace std;

int main()
{
    CFG cfg;
    
    int n;
    cin >> n;
    for (int i = 0; i < n; i++) {
        char symbol;
        cin >> symbol;
        cfg.addN(symbol);
    }
    cin >> n;
    for (int i = 0; i < n; i++) {
        char symbol;
        cin >> symbol;
        cfg.addT(symbol);
    }
    cin >> n;
    for (int i = 0; i < n; i++) {
        char symbol;
        char str[20];
        cin >> symbol;
        cin.get();
        cin.getline(str, 20);
        cfg.addP(symbol, str);
    }

    char symbol;
    cin >> symbol;
    cfg.setInitSymb(symbol);

    cfg.removeDeltaTrans();
    cfg.removeSingleTrans();
    cfg.removeNoUseSymb();
    cfg.transformToCNF();
    cfg.transformToGNF();

    cout << cfg;
    return 0;
}
