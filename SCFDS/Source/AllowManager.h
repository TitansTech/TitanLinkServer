
#define ALISTFILE			".//Data//TT_AllowedList.dat"
#define MAXIPINALLOWLIST	10

class cAllowManager
{
public:
	bool IPCheck(LPSTR IP);
	void ReadAllowList(LPSTR filename);
	//

private:	
	//
	void AllowListClean();
	bool AddToAllowList(LPSTR IP);
	char AllowListIP[MAXIPINALLOWLIST][16];
	int ALSize;
};

extern cAllowManager AllowManager;
//----------------------------------------