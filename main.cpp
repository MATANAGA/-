#include <Novice.h>
#include <string>
#include <vector>
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
const char kWindowTitle[] = "一匹残らずゴブリン討伐";
const int kWindowWidth = 13 * 64 + 300; //screen width
const int kWindowHeight = 13 * 64; //screen height
const int kBlockSize = 64; // マップチップサイズ（X）
const int kPlayerRadius = kBlockSize / 2;
const int KBlockwidth = 13;
const int kBlockHeight = 13;
const int floorCount = 20;
int currentFloor = 0; // 当前楼层索引

bool isItemDialogActive = false;  // 是否显示拾取道具的对话框
std::string itemDialogMessage = ""; // 当前对话框的显示信息


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// シーン定義

// タイトルの選択肢
enum TitleOption { START_GAME, GAME_INSTRUCTIONS, END_GAME, MAX_OPTIONS };

// モード選択肢
enum ModeOption { EASY_MODE, NORMAL_MODE, MAX_MODES };

struct Option {
	int x, y;  // 描画位置
	const char* text;
};

// タイトル画面の選択肢定義
Option options[MAX_OPTIONS] = {
	{550, 400, "Start Game"},
	{550, 500, "Instructions"},
	{550, 600, "Exit"}
};

// モード選択肢定義
Option modeOptions[MAX_MODES] = {
	{475, 300, "Easy Mode"},
	{475, 400, "Normal Mode"}
};

// 透明度を制御
int transparency[MAX_OPTIONS] = { 255, 255, 255 };
int modeTransparency[MAX_MODES] = { 255, 128 };

// スプライトハンドル
int arrowSprite;
int optionSprites[MAX_OPTIONS];
int modeSprites[MAX_MODES];
/////////////////////////////////////////////////////



const int kItemDialogCount = 9; // 对话框图片数量
int itemDialogImage[kItemDialogCount]; // 定义对话框图片数组
int currentItemDialogIndex = -1; // 当前对话框图片索引，-1 表示无图片
int goldDialogImage; // 新增金币对话框图片
bool isGoldDialogActive = false; // 金币对话框是否激活
int goldDialogTimer = 0;         // 金币对话框计时器
int earnedGold = 0;              // 本次获得的金币数量
const int goldDialogDuration = 60; // 持续时间（60帧为1秒）



int itemDialogTimer = 0;  // 计时器
const int itemDialogDuration = 60;  // 持续时间（60帧为1秒）

const int kDigitCount = 10;    // 数字的数量（0-9）
std::vector<int> numberTextures; // 数字纹理数组
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

int fairyDialogueCooldownTimer = 0; // 冷却时间计数器，初始为 0
const int fairyDialogueCooldownDuration = 300; // 冷却时间（300帧，假设帧率为60，即5秒）

const int dialogueCount = 4;
int fairyDialogue[dialogueCount];
// 新增变量                        
bool isFairyDialogueActive = false; // 是否处于仙女对话模式
int currentFairyDialogueIndex = 0;  // 当前对话索引
//////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////
// 商店选项
enum ShopOption { INCREASE_ATTACK, INCREASE_DEFENSE, RESTORE_HEALTH, EXIT_SHOP, MAX_SHOP_OPTIONS };

// 商店状态变量
bool isShopActive = false;       // 是否进入商店
int selectedShopOption = 0;      // 当前选中的商店选项
int shopCooldownTimer = 0;       // 商店冷却时间计时器
const int shopCooldownDuration = 300; // 冷却时间（300帧）
int shopTransparency[MAX_SHOP_OPTIONS] = { 255, 128, 128, 128 }; // 初始透明度
// 商店选项贴图和透明度
int shopSprites[MAX_SHOP_OPTIONS];      // 商店选项贴图
int shopMur;
// 商店选项价格及增长
int shopOptionPrices[MAX_SHOP_OPTIONS] = { 20, 20, 20, 0 }; // 每个选项初始价格
const int priceIncrement = 5; // 每次购买价格增加的值

void DrawShopMenu() {
	// 商店选项的位置
	Option shopOptions[MAX_SHOP_OPTIONS] = {
		{285, 150, "Increase Attack"},
		{285, 250, "Increase Defense"},
		{285, 350, "Restore Health"},
		{285, 450, "Exit Shop"}
	};

	// 绘制商店背景
	Novice::DrawSprite(0, 0, shopMur, 1, 1, 0, WHITE);
	// 绘制商店选项及价格
	for (int i = 0; i < MAX_SHOP_OPTIONS; ++i) {
		Novice::DrawSprite(shopOptions[i].x, shopOptions[i].y, shopSprites[i], 1.0f, 1.0f, 0.0f, shopTransparency[i]);


	}

	// 绘制选中的箭头
	int arrowX = shopOptions[selectedShopOption].x - 50;
	int arrowY = shopOptions[selectedShopOption].y;
	Novice::DrawSprite(arrowX, arrowY, arrowSprite, 1.0f, 1.0f, 0.0f, 255);
}

////////////////////////////////////////////////////////////////////////////////
bool isSceneTransition = false;  // 是否正在进行场景切换
int transitionTimer = 0;         // 过渡计时器
const int transitionDuration = 30; // 黑屏持续时间（60帧为1秒）
bool isTransitionFadeOut = true; // true 表示渐暗，false 表示渐亮


/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void LoadNumberTextures() {
	std::string basePath = "./Images/Numbers/"; // 假设图片路径为 Images/Numbers 目录
	for (int i = 0; i < kDigitCount; ++i) {
		std::string filePath = basePath + std::to_string(i) + ".png";
		numberTextures.push_back(Novice::LoadTexture(filePath.c_str()));
	}
}

void DrawNumber(int number, int x, int y, int digitWidth, int digitHeight, unsigned int color) {
	std::string numberStr = std::to_string(number);
	// 移除前导零（保留至少一个数字）
	size_t nonZeroPos = numberStr.find_first_not_of('0');
	if (nonZeroPos == std::string::npos) {
		numberStr = "0"; // 如果全是零，保留一个零
	}
	else {
		numberStr = numberStr.substr(nonZeroPos);
	}

	for (int i = 0; i < numberStr.length(); ++i) {
		int digit = numberStr[i] - '0';
		if (digit >= 0 && digit < kDigitCount) {
			float scaleX = digitWidth / 32.0f; // 假设数字图片原始宽度为 32 像素
			float scaleY = digitHeight / 64.0f; // 假设数字图片原始高度为 64 像素
			Novice::DrawSprite(x + i * digitWidth, y, numberTextures[digit], scaleX, scaleY, 0.0f, color);
		}
	}
}



// Map chip types
enum MapChip {
	Empty = 0,       // 空地
	Brick = 1,       // 墙体
	Monsters = 2,    // 通用怪物标识符（可以用来生成随机怪物）
	RedPotion = 3,   // 红药水
	BluePotion = 4,  // 蓝药水
	AttackGem = 5,   // 攻击宝石
	DefenseGem = 6,  // 防御宝石
	RedKey = 7,      // 红钥匙
	BlueKey = 8,     // 蓝钥匙
	GreenKey = 9,    // 绿钥匙
	RedDoor = 10,    // 红门
	BlueDoor = 11,   // 蓝门
	GreenDoor = 12,  // 绿门
	stairsUp = 13,   // 向上的楼梯
	stairsDown = 14, // 向下的楼梯
	blood = 15,      // 怪物死亡后的血迹
	BabyGoblin = 16, //怪物
	Goblin = 17,
	GoblinLeader = 18,
	GoblinArcher = 19,
	GoblinAssassin = 20,
	GoblinKnight = 21,
	GoblinKing = 22,
	GoblinHero = 23,
	Knight = 25,//NPC
	Elderly = 26,
	KeyMerchant = 27,
	Merchant = 28,
	Deceased = 29,
	Fairy = 36,//NPC
	IronSword = 30,//剑盾
	IronShield = 31,
	KnightSword = 32,
	KnightShield = 33,
	BraveSword = 34,
	BraveShield = 35,//剑盾
	End = 37,
};

int killedMonsterCount = 0;

enum scene {
	Title,
	GameOver,
	Clear,
	MonsterManual,
	Game,
	Menu,
	Battle, // 新增战斗场景
	MODE_SELECTION,
	RESULT,
	Help
};


// Player attributes
struct Player {
	int posX, posY;
	int health;
	int attack;
	int defense;
	int redKeys, blueKeys, greenKeys;
	int gold; // 增加金币变量
	int direction; // 方向 0:下, 1:上, 2:左, 3:右
	int frame;
	bool swordGet;
	bool shieldGet;
};


struct Monster {
	const char* name;  // 怪物名字
	int health;        // 生命值
	int initialHealth; // 初始生命值
	int attack;        // 攻击力
	int defense;       // 防御力
	int gold;          // 掉落金币
	int posX, posY;    // 怪物位置
	bool alive;        // 怪物是否存活
};



Monster monsterManual[] = {
	{"Baby Goblin", 50, 50, 20, 0, 2, 0, 0, true}, // MapChip = 16
	{"Goblin",      80, 80, 28, 4, 4, 0, 0, true},      // MapChip = 17
	{"Goblin Leader", 150, 150, 35, 20, 10, 0, 0, true}, // MapChip = 18
	{"Goblin Archer", 60, 60, 40, 0, 4, 0, 0, true}, // 其他怪物以此类推
	{"Goblin Assassin", 40, 40, 50, 5, 5, 0, 0, true}, // MapChip = 20
	{ "Goblin Knight", 200, 200, 60, 35, 50, 0, 0, true }, // MapChip = 21
	{ "Goblin King", 500, 500, 80, 30, 100, 0, 0, true }, // MapChip = 21
	{ "Goblin Hero", 1000, 1000, 100, 30, 0, 0, 0, true }, // MapChip = 21

};





Player localPlayer = {
	kBlockSize + kPlayerRadius * 11, kBlockSize + kPlayerRadius * 23,
	100,  // 初始生命值
	5,    // 初始攻击力
	2,    // 初始防御力
	5, 5, 5,  // 钥匙数量
	0,     // 初始金币数量
	0,
	0,
	false,
	false
};


bool trueEnd = false;
bool shieldBgm = false;
bool swordBgm = false;
bool bossBgm = false;
bool hasPlayedBgm = false;  // 添加一个标志位，表示BGM是否已经播放过
scene currentScene = Title;
int bgmTitlePlayHandle = -1;
int bgmDifficultySelectPlayHandle = 0;
int bgmGamePlayHandle = 0;
int bgmGetSwordShieldPlayHandle = 0;
int bgmClearPlayHandle = 0;
int bgmBossPlayHandle = 0;

int npcClass = Knight;
bool isInteracting;      // 是否正在与该 NPC 互动


int enemyClass = BabyGoblin;
bool inBattle = false;     // 是否处于战斗状态
Monster* currentEnemy = nullptr; // 当前敌人指针
int battleFrameCounter = 0; // 战斗帧计数器

int map[floorCount][kBlockHeight][KBlockwidth] = {
	// 第一层
	{
		{1, 1,  1,  1,  1 , 1,  1,  1,  1,  1,  1,  1,  1},
		{1, 4,  16, 17, 2 , 2,  2,  5,  5,  8,  3,  4,  1},
		{1, 8,  3,  6,  1 , 1,  1,  1,  1,  1,  1,  1,  1},
		{1, 9,  20, 6,  1 , 0,  3,  0,  0,  0,  0,  36, 1},
		{1, 1,  11, 1,  1 , 0,  1,  1,  1,  0,  0,  0,  1},              //  0   Empty,
		{1, 0,  0,  0,  1 , 0,  1,  5,  1,  16, 1,  0,  1},			   //  1   Brick,
		{1, 0,  0,  0,  1 , 0,  1,  6,  1,  19, 1,  0,  1},			   //  2   Monsters,
		{1, 19, 0,  0,  2 , 0,  1, 16,  1,  16, 20, 0,  1},			   //  3   RedPotion,
		{1, 11, 1,  1,  1 , 0,  0, 12,  1,  0,  1,  0,  1},			   //  4   BluePotion,
		{1, 9,  16,  9,  1 , 1, 10,  1,  1, 1 , 1,  11, 1},			   //  5   AttackGem,
		{1, 3,  8,  3,  1 , 7,  0,  8,  1,  9,  9,  0,  1},			   //  6   DefenseGem,
		{1, 5,  16, 5,  1 , 9,  0,  9,  1,  9,  0,  13, 1},
		{1, 1,  1,  1,  1 , 4,  0,  3,  1,  1,  1,  1,  1},
		},	                                                   //  　　　　　7   RedKey,
		//  　　　　　　　　　　　7   RedKey,

// 　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 8   BlueKey,
// 　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 9   GreenKey,
// 　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 10  RedDoor,
// 　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 11  BlueDoor,
// 　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 12  GreenDoor,
   //  　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　13  stairsUp,
// 第二层															   //  14  stairsDown
{
	{1, 1,  1, 1,  1 , 1,  1,  1,  1,  1,  1,  1,  1},
	{1, 14, 1, 5,  5 , 5,  1,  7,  1,  3,  3,  3,  1},
	{1, 0,  1, 7,  9 , 9,  1,  7,  1,  8,  8,  8,  1},
	{1, 0,  1, 9,  9 , 9,  10, 18, 10, 6,  6,  6,  1},
	{1, 0,  1, 6,  6 , 6,  1,  0,  1,  4,  4,  4,  1},
	{1, 0,  1, 1,  1 , 1,  1,  8,  1,  1,  1,  1,  1},
	{1, 8, 10, 6,  6 , 4,  4,  0, 20,  5,  5,  5,  1},
	{1, 9,  1, 1,  1 , 1,  1,  8,  1,  1,  1,  1,  1},
	{1, 4,  1, 3,  3 , 3,  1,  0,  1,  5,  5,  3,  1},
	{1, 4,  1, 3,  3 , 3, 10, 21, 10,  6,  6,  3,  1},
	{1, 0,  1, 9,  9 , 9,  1,  7,  1,  3,  3,  3,  1},
	{1, 13, 1, 8,  8 , 8,  1,  7,  1,  3,  3,  3,  1},
	{1, 1,  1, 1,  1 , 1,  1,  1,  1,  1,  1,  1,  1},
},
// 第三层		
// 
													  // 第si层															   //  14  stairsDown
{
	{1, 1,   1,  1,  1 , 1,  1,  1,  1,  1,  1,  1,  1},
	{1, 5,   1,  3,  9 , 1,  14, 1,  5,  0,  17,  4,  1},
	{1, 6,   1,  6, 16 , 1,  0,  1,  3,  17,  17,  5,  1},
	{1, 0,   8,  1, 12 ,17, 12,  1,  1,  12,  1,  1,  1},
	{1, 16,  9,  1,  0 , 1, 12,  1,  0,   0, 16,  0,  1},
	{1, 16,  1,  5,  0 , 1, 12,  12,  0,  1,  1,  12,  1},
	{1, 16, 12,  5,  1 , 1,  0,  1,  0,   0, 17,  0,  1},
	{1, 12,  1,  0,  1 ,28,  0,  1,  0,  1,  1,  0,  1},
	{1, 17,  1,  0,  0 , 0,  0,  1, 16,  1,  1, 11,  1},
	{1, 17,  1,  1, 17 , 1,  0,  1, 12,  6,  1,  9,  1},
	{1, 6,   1,  8,  9 , 1,  0,  1,  8,  4,  1,  8,  1},
	{1, 6,   1,  4,  4 , 1,  13, 1,  9,  9,  1,  7,  1},
	{1, 1,   1,  1,  1 , 1,  1,  1,  1,  1,  1,  1,  1},
},
//4
{
	{1, 1,  1,  1,  1 , 1,  1,  1,  1,  1,  1,  1,  1},
	{1, 14, 0,  1,  0 , 5,  4,  3,  0,  1,  3,  3,  1},
	{1, 0,  0,  1,  0 , 1,  1,  1, 16, 17,  0,  0,  1},
	{1, 0,  0,  1,  17, 1, 30,  1,  9,  1,  1, 11,  1},
	{1, 1, 12,  1,  16,16, 11,  1,  9,  1,  6,  3,  1},
	{1, 4,  0,  1,  1 , 1, 11,  1, 16,  1,  1,  1,  1},
	{1, 1,  0, 12,  0 , 0,  0,  0,  0,  1,  9,  8,  1},
	{1,12, 12,  1, 12 , 1,  1,  1,  0,  1,  1, 20,  1},
	{1, 0,  1,  1, 11 ,17, 17,  1,  0,  1,  0,  0,  1},
	{1,16,  9,  1,  1 , 1,  0,  1,  0, 18,  0,  1,  1},
	{1, 1,  9,  1,  3 , 5,  3,  1,  0,  1,  0,  0,  1},
	{1, 5,  6,  1,  3 , 5,  3,  1,  0, 12, 16, 13,  1},
	{1, 1,  1,  1,  1 , 1,  1,  1,  1,  1,  1,  1,  1},
},
//5
{
	{1, 1,  1,  1,  1 , 1,  1,  1,  1,  1,  1,  1,  1},
	{1, 14, 0, 17,  0 , 0,  0,  1,  0, 18, 18,  0,  1},
	{1, 0,  0, 19,  0 , 0,  0, 12,  0, 12, 12,  0,  1},
	{1, 1,  1,  1,  1 , 0,  0,  1,  1,  1,  1,  0,  1},
	{1, 8,  6, 17, 11 ,12,  0,  1,  5,  9,  9,  0,  1},
	{1, 1,  1,  0,  1 , 1,  0,  1,  1, 11,  1,  1,  1},
	{1, 6,  1, 17,  0 , 1, 18,  0,  1,  0,  0,  0,  1},
	{1, 3,  1, 17,  4 , 1,  0,  0,  1, 18,19,  20,  1},
	{1, 9,  1,  7,  0 , 1,  0,  0, 11,  0,  0,  0,  1},
	{1, 9,  1,  1,  1 , 1,  0,  1, 12,  3,  5,  0,  1},
	{1,17,  1,  5,  5 , 1,  0,  1, 12,  1,  1,  0,  1},
	{1, 0, 12,  0, 17 ,12,  0,  0,  0,  0,  0, 13,  1},
	{1, 1,  1,  1,  1 , 1,  1,  1,  1,  1,  1,  1,  1},
},
//6
{
	{1, 1,  1,  1,  1 , 1,  1,  1,  1,  1,  1,  1,  1},
	{1, 3,  5,  6,  1 , 0, 14,  0,  0,  1,  9,  9,  1},
	{1, 3,  5,  6,  1 , 0,  0,  0,  0,  1,  9,  9,  1},
	{1, 7,  3,  18,12 , 0,  1,  1,  20, 20,  5,  6,  1},
	{1, 9,  4,  4,  1 , 0,  1,  0,  20, 1,  3,  3,  1},
	{1, 1,  1,  1,  1 , 0,  1,  0,  20, 1,  1,  1,  1},
	{1,20,  4, 11, 11 , 0,  1,  0,  20, 0,  0,  0,  1},
	{1,17,  1,  0,  1 ,17,  1,  0,  20, 0,  1, 18,  1},
	{1,20,  1,  0,  1 , 0,  0,  1,  20, 0,  1, 18,  1},
	{1, 0,  1, 31,  1 , 0,  0,  1,  20, 0, 11,  0,  1},
	{1, 0,  1,  1,  1 , 0,  0,  1,  1,  1,  1,  0,  1},
	{1, 0,  0,  0,  0 , 0,  0,  0,  0,  0,  0, 13,  1},
	{1, 1,  1,  1,  1 , 1,  1,  1,  1,  1,  1,  1,  1},
},
//7
{
	{1, 1,  1,  1,  1 , 1,  1,  1,  1,  1,  1,  1,  1},
	{1, 0,  0,  0,  0 , 0, 10,  0,  0,  0,  0,  0,  1},
	{1, 0,  0,  1,  1 , 1, 10,  1,  1,  1,  0,  0,  1},
	{1, 0,  1,  17, 16 ,1, 13,  1, 16, 17, 1,  0,  1},
	{1, 0,  1,  0, 17 , 1, 21,  1,  17, 0,  1,  0,  1},
	{1, 0,  1,  0,  0 ,21, 22, 21,  0,  0,  1,  0,  1},
	{1, 0,  1, 17, 17 , 0, 21,  0, 17, 17,  1,  0,  1},
	{1, 0,  1,  0, 17 , 0,  0,  0, 17,  0,  1,  0,  1},
	{1, 0,  1,  0,  0 ,17,  0, 17,  0,  0,  1,  0,  1},
	{1, 0,  1,  0, 17 ,18,  0, 18, 17,  0,  1,  0,  1},
	{1, 0, 28,  1,  1 , 1,  0,  1,  1,  1, 14,  0,  1},
	{1, 0,  0,  0,  0 , 0,  0,  0,  0,  0,  0,  0,  1},
	{1, 1,  1,  1,  1 , 1,  1,  1,  1,  1,  1,  1,  1},
},
//8
{
	{1, 1,  1,  1,   1 , 1,  1,  1,  1,  1,  1,  1,  1},
	{1, 16, 16, 16,  0 , 1,  14, 1,  0, 16, 16, 16,  1},
	{1, 16, 16, 16,  1 , 1,  0,  1,  1, 16, 16, 16,  1},
	{1, 16, 0,  16, 16 , 1,  0,  1, 16, 16, 16  ,0 , 1},
	{1, 16, 16, 16, 17 , 1,  0,  1, 17, 16, 16, 16,  1},
	{1, 0,  16, 16,  0 , 0,  0,  0,  0, 16, 16, 16,  1},
	{1, 16, 16, 16, 17 , 1,  0,  1,  17,16, 16, 16,  1},
	{1, 16, 16, 16, 17 , 1,  0,  1,  17,16, 16, 16,  1},
	{1, 16, 16, 16,  0 , 1,  0,  1,  0, 16, 16, 16,  1},
	{1, 16, 16, 16, 17 , 1,  0,  1,  17, 16,16, 16,  1},
	{1, 16, 16, 16,  1 , 1,  0,  1,  1, 16, 16, 16,  1},
	{1, 16, 16, 16,  0 , 1,  0,  1,  0, 16, 16, 16,  1},
	{1, 1,  1,  1,  1 ,  1, 37,  1,  1,  1,  1,  1,  1},
},

};

int enemyCount = 0;
int enemyFrame = 0;

void DrawMonsterManual() {
	int x = 50, y = 50; // 初始显示位置
	int spacing = 100;  // 每个怪物的间隔
	int imageSize = 100; // 贴图大小
	int manual[] = {
	Novice::LoadTexture("./Images/intelligence.png"),
	Novice::LoadTexture("./Images/trun.png"),
	Novice::LoadTexture("./Images/dmage.png"),
	Novice::LoadTexture("./Images/hlel.png"),


	};
	int monsterImages[] = {
	Novice::LoadTexture("./Images/baybyGoblin1.png"),
	Novice::LoadTexture("./Images/goblin1.png"),
	Novice::LoadTexture("./Images/goblin_leader.png"),
	Novice::LoadTexture("./Images/goblin_Archer1.png"),
	Novice::LoadTexture("./Images/goblin_Assassin2.png"),
	Novice::LoadTexture("./Images/goblin_Knight1.png"),
	Novice::LoadTexture("./Images/goblin_King1.png"),
	Novice::LoadTexture("./Images/goblin_Hero2.png"),

	};
	int monsterNames[] = {
	   Novice::LoadTexture("./Images/name_bayby.png"),
	   Novice::LoadTexture("./Images/name_goblin.png"),
	   Novice::LoadTexture("./Images/name_goblin_leader.png"),
	   Novice::LoadTexture("./Images/name_goblin_Archer.png"),
	   Novice::LoadTexture("./Images/name_goblin_Assassin.png"),
	   Novice::LoadTexture("./Images/name_goblin_Knight.png"),
	   Novice::LoadTexture("./Images/name_goblin_King.png"),
	Novice::LoadTexture("./Images/name_goblin_hero.png"),


	};
	Novice::DrawSprite(x, y, manual[0], 1, 1, 0, WHITE);
	y += 50; // 移动到下一行

	for (int i = 0; i < sizeof(monsterManual) / sizeof(Monster); ++i) {
		Monster& monster = monsterManual[i];

		// 检查怪物是否在当前楼层的地图中存在
		bool isMonsterPresent = false;
		for (int row = 0; row < kBlockHeight; ++row) {
			for (int col = 0; col < KBlockwidth; ++col) {
				if (map[currentFloor][row][col] == (BabyGoblin + i)) {
					isMonsterPresent = true;
					break;
				}
			}
			if (isMonsterPresent) break;
		}

		if (!isMonsterPresent) continue; // 如果当前楼层没有此怪物，跳过

		// 绘制怪物贴图
		Novice::DrawSprite(x, y, monsterImages[i], 0.5f, 0.5f, 0, WHITE);

		// 绘制怪物的名称贴图
		int nameX = x + imageSize + 20;
		Novice::DrawSprite(nameX - 17, y - 10, monsterNames[i], 1, 1, 0, WHITE);

		// 绘制怪物的战斗信息
		int infoX = x + imageSize + 20; // 信息显示位置

		Novice::DrawSprite(nameX - 7, y + 20, manual[3], 1, 1, 0, WHITE);
		DrawNumber(monster.health, nameX + 50, y + 25, 15, 15, 0x00ccccFF); // 主角血量
		DrawNumber(monster.attack, nameX + 150, y + 25, 15, 15, 0x00ccccFF); // 主角血量
		DrawNumber(monster.defense, nameX + 250, y + 25, 15, 15, 0x00ccccFF); // 主角血量
		DrawNumber(monster.gold, nameX + 360, y + 25, 15, 15, 0x00ccccFF); // 主角血量

		//Novice::ScreenPrintf(infoX, y + 20, "HP: %d | ATK: %d | DEF: %d | Gold: %d",
			//monster.health, monster.attack, monster.defense, monster.gold);

		// 计算玩家对怪物造成的伤害
		int damageToMonsterPerHit = localPlayer.attack - monster.defense;
		if (damageToMonsterPerHit < 1) damageToMonsterPerHit = 1;

		// 计算怪物对玩家造成的伤害
		int damageToPlayerPerHit = monster.attack - localPlayer.defense;
		if (damageToPlayerPerHit < 1) damageToPlayerPerHit = 0;

		// 计算主角击败怪物所需回合数
		int hitsToDefeatMonster = (monster.health + damageToMonsterPerHit - 1) / damageToMonsterPerHit;

		// 修正怪物对玩家造成的总伤害
		int turnsMonsterCanAttack = hitsToDefeatMonster - 1; // 怪物只能在玩家击败前的回合攻击
		int totalDamageToPlayer = damageToPlayerPerHit * turnsMonsterCanAttack;

		// 显示计算结果
		Novice::DrawSprite(infoX - 3, y + 50, manual[1], 1, 1, 0, WHITE);
		DrawNumber(hitsToDefeatMonster, infoX + 133, y + 55, 15, 15, RED); // 主角血量

		//Novice::ScreenPrintf(infoX, y + 40, "Turns to Defeat: %d", hitsToDefeatMonster);
		Novice::DrawSprite(x + 310, y + 50, manual[2], 1, 1, 0, WHITE);
		DrawNumber(totalDamageToPlayer, x + 425, y + 55, 15, 15, RED); // 主角血量


		// 下一行位置
		y += spacing;
	}
}



int initialMapState[floorCount][kBlockHeight][KBlockwidth];

void InitializeGame() {
	// 初始化玩家属性
	localPlayer = {
		kBlockSize + kPlayerRadius * 11, kBlockSize + kPlayerRadius * 23,
		100,  // 初始生命值
		5,    // 初始攻击力
		2,    // 初始防御力
		5, 5, 5,  // 钥匙数量
		0     // 初始金币数量
	};

	// 初始化所有怪物
	for (int i = 0; i < sizeof(monsterManual) / sizeof(Monster); ++i) {
		monsterManual[i].health = monsterManual[i].initialHealth;
		monsterManual[i].alive = true;
	}

	// 重置地图
	memcpy(map, initialMapState, sizeof(map));

	// 初始化其他游戏状态
	currentFloor = 0;
	isItemDialogActive = false;
	isShopActive = false;
	isSceneTransition = false;
	transitionTimer = 0;
	inBattle = false;
	currentEnemy = nullptr;
	localPlayer.shieldGet = false;
	localPlayer.swordGet = false;
	hasPlayedBgm = false;  // 标记BGM已经播放过
	killedMonsterCount = 0;

}




bool CheckCollision(int x, int y) {
	int mapX = x / kBlockSize;
	int mapY = y / kBlockSize;

	// 防止顶部超出地图范围
	if (mapX < 0 || mapY < 0 || mapX >= KBlockwidth || mapY >= kBlockHeight) {
		return false; // 超出地图范围
	}

	// 判断该位置是否可以通过（包括普通地面、药水、宝石、楼梯等）
	if (map[currentFloor][mapY][mapX] == Empty ||
		map[currentFloor][mapY][mapX] == RedPotion ||
		map[currentFloor][mapY][mapX] == BluePotion ||
		map[currentFloor][mapY][mapX] == AttackGem ||
		map[currentFloor][mapY][mapX] == DefenseGem ||
		map[currentFloor][mapY][mapX] == stairsDown ||
		map[currentFloor][mapY][mapX] == stairsUp ||
		map[currentFloor][mapY][mapX] == RedKey ||
		map[currentFloor][mapY][mapX] == blood ||
		map[currentFloor][mapY][mapX] == BabyGoblin ||
		map[currentFloor][mapY][mapX] == Goblin ||
		map[currentFloor][mapY][mapX] == GoblinLeader ||
		map[currentFloor][mapY][mapX] == GoblinArcher ||
		map[currentFloor][mapY][mapX] == GoblinAssassin ||
		map[currentFloor][mapY][mapX] == GoblinKnight ||
		map[currentFloor][mapY][mapX] == GoblinKing ||
		map[currentFloor][mapY][mapX] == GoblinHero ||
		map[currentFloor][mapY][mapX] == Monsters ||
		map[currentFloor][mapY][mapX] == BlueKey ||
		map[currentFloor][mapY][mapX] == GreenKey ||
		map[currentFloor][mapY][mapX] == IronShield ||
		map[currentFloor][mapY][mapX] == IronSword ||
		map[currentFloor][mapY][mapX] == BraveSword ||
		map[currentFloor][mapY][mapX] == KnightSword ||
		map[currentFloor][mapY][mapX] == BraveShield ||
		map[currentFloor][mapY][mapX] == KnightShield ||
		map[currentFloor][mapY][mapX] == Knight ||
		map[currentFloor][mapY][mapX] == Elderly ||
		map[currentFloor][mapY][mapX] == KeyMerchant ||
		map[currentFloor][mapY][mapX] == Merchant ||
		map[currentFloor][mapY][mapX] == Deceased ||
		map[currentFloor][mapY][mapX] == End ||
		map[currentFloor][mapY][mapX] == Fairy)


	{
		return true; // 玩家可以走到这些位置
	}

	// 如果玩家有红钥匙
	if (localPlayer.redKeys > 0) {
		if (map[currentFloor][mapY][mapX] == RedDoor) {
			return true; // 玩家可以打开红色门
		}
	}

	// 如果玩家有蓝钥匙
	if (localPlayer.blueKeys > 0) {
		if (map[currentFloor][mapY][mapX] == BlueDoor) {
			return true; // 玩家可以打开蓝色门
		}
	}

	// 如果玩家有绿钥匙
	if (localPlayer.greenKeys > 0) {
		if (map[currentFloor][mapY][mapX] == GreenDoor) {
			return true; // 玩家可以打开绿色门
		}
	}

	// 如果不符合任何条件，返回 false
	return false;



}




// マップチップの当たり判定
void InteractWithChip(int x, int y) {
	int mapX = x / kBlockSize;
	int mapY = y / kBlockSize;

	int baoshiSE = Novice::LoadAudio("./Sounds/baoshi.wav");
	int yaoshuiSE = Novice::LoadAudio("./Sounds/10.wav");
	int kaimenSE = Novice::LoadAudio("./Sounds/y973.mp3");
	int zhaohuSE = Novice::LoadAudio("./Sounds/r_a02.wav");

	switch (map[currentFloor][mapY][mapX]) {
	case RedPotion:
		localPlayer.health += 50;
		currentItemDialogIndex = 0; // 红药水对话框
		isItemDialogActive = true;
		Novice::PlayAudio(yaoshuiSE, false, 1.0f);

		itemDialogTimer = itemDialogDuration; // 启动计时器
		map[currentFloor][mapY][mapX] = Empty;
		break;
	case BluePotion:
		localPlayer.health += 200;
		currentItemDialogIndex = 1;
		isItemDialogActive = true;
		Novice::PlayAudio(yaoshuiSE, false, 1.0f);
		itemDialogTimer = itemDialogDuration;
		map[currentFloor][mapY][mapX] = Empty;
		break;
	case AttackGem:
		localPlayer.attack += 1;
		currentItemDialogIndex = 2;
		isItemDialogActive = true;
		Novice::PlayAudio(baoshiSE, false, 1.0f);
		itemDialogTimer = itemDialogDuration;
		map[currentFloor][mapY][mapX] = Empty;
		break;
	case DefenseGem:
		localPlayer.defense += 1;
		currentItemDialogIndex = 3;
		Novice::PlayAudio(baoshiSE, false, 1.0f);
		isItemDialogActive = true;
		itemDialogTimer = itemDialogDuration;
		map[currentFloor][mapY][mapX] = Empty;
		break;
	case RedKey:
		localPlayer.redKeys++;
		currentItemDialogIndex = 6;
		isItemDialogActive = true;
		itemDialogTimer = itemDialogDuration;

		map[currentFloor][mapY][mapX] = Empty;
		break;
	case BlueKey:
		localPlayer.blueKeys++;
		currentItemDialogIndex = 5;
		isItemDialogActive = true;
		itemDialogTimer = itemDialogDuration;

		map[currentFloor][mapY][mapX] = Empty;
		break;
	case GreenKey:
		localPlayer.greenKeys++;
		currentItemDialogIndex = 4;
		isItemDialogActive = true;
		itemDialogTimer = itemDialogDuration;

		map[currentFloor][mapY][mapX] = Empty;
		break;


	case RedDoor:
		if (localPlayer.redKeys > 0) {
			localPlayer.redKeys--;
			Novice::PlayAudio(kaimenSE, false, 1.0f);

			map[currentFloor][mapY][mapX] = Empty;
		}
		break;
	case BlueDoor:
		if (localPlayer.blueKeys > 0) {
			localPlayer.blueKeys--;
			Novice::PlayAudio(kaimenSE, false, 1.0f);

			map[currentFloor][mapY][mapX] = Empty;
		}
		break;
	case GreenDoor:
		if (localPlayer.greenKeys > 0) {
			localPlayer.greenKeys--;
			Novice::PlayAudio(kaimenSE, false, 1.0f);

			map[currentFloor][mapY][mapX] = Empty;
		}
		break;

	case stairsUp:

		if (currentFloor < floorCount - 1 && !isSceneTransition) {
			isSceneTransition = true;  // 开始场景切换
			isTransitionFadeOut = true; // 从场景渐暗开始
			transitionTimer = 0;       // 重置计时器
		}
		if (currentFloor < floorCount - 1) {
			currentFloor++;  // 切换楼层
			// 立即将玩家的位置设为新楼层的楼梯旁边的位置
			for (int i = 0; i < kBlockHeight; i++) {
				for (int j = 0; j < KBlockwidth; j++) {
					if (map[currentFloor][i][j] == stairsDown) {
						// 假设玩家出现在楼梯的右侧
						localPlayer.posX = j * kBlockSize + kPlayerRadius;  // 玩家在楼梯的右侧
						localPlayer.posY = (i + 1) * kBlockSize + kPlayerRadius;        // 高度不变
						return; // 找到新楼层的楼梯旁边并设置玩家位置
					}
				}
			}
		}
		break;

	case stairsDown:

		if (currentFloor > 0 && !isSceneTransition) {
			isSceneTransition = true;
			isTransitionFadeOut = true;
			transitionTimer = 0;
		}
		if (currentFloor > 0) {
			currentFloor--;  // 切换楼层
			// 立即将玩家的位置设为新楼层的楼梯旁边的位置
			for (int i = 0; i < kBlockHeight; i++) {
				for (int j = 0; j < KBlockwidth; j++) {
					if (map[currentFloor][i][j] == stairsUp) {
						// 假设玩家出现在楼梯的下方
						localPlayer.posX = j * kBlockSize + kPlayerRadius;        // 横坐标不变
						localPlayer.posY = (i - 1) * kBlockSize + kPlayerRadius;  // 玩家在楼梯的下方
						return; // 找到新楼层的楼梯旁边并设置玩家位置
					}
				}
			}
		}
		break;

	case BabyGoblin:
	case Goblin:
	case GoblinLeader:
	case GoblinArcher:
	case GoblinAssassin:
	case GoblinKnight:
	case GoblinKing:
	case GoblinHero:

		currentEnemy = &monsterManual[map[currentFloor][mapY][mapX] - BabyGoblin]; // 计算对应怪物
		Novice::ScreenPrintf(200, 200, "Encountered %s!", currentEnemy->name);
		inBattle = true; // 启动战斗模式
		break;


	case Knight:
	case Elderly:
	case KeyMerchant:
	case Fairy:
		if (!isFairyDialogueActive && fairyDialogueCooldownTimer == 0) {
			isFairyDialogueActive = true; // 激活对话模式
			currentFairyDialogueIndex = 0; // 从第一段对话开始

			// 根据楼层设置不同的对话
			if (currentFloor == 0) {
				fairyDialogue[0] = Novice::LoadTexture("./Images/Floor0Dialog1.png");
				fairyDialogue[1] = Novice::LoadTexture("./Images/Floor0Dialog2.png");
				fairyDialogue[2] = Novice::LoadTexture("./Images/Floor0Dialog3.png");
				fairyDialogue[3] = Novice::LoadTexture("./Images/Floor0Dialog4.png");
			}
			else if (currentFloor == 1) {
				fairyDialogue[0] = Novice::LoadTexture("./Images/Floor1Dialog1.png");
				fairyDialogue[1] = Novice::LoadTexture("./Images/Floor1Dialog2.png");
				fairyDialogue[2] = Novice::LoadTexture("./Images/Floor1Dialog3.png");
				fairyDialogue[3] = Novice::LoadTexture("./Images/Floor1Dialog4.png");
			}
			else if (currentFloor == 2) {
				fairyDialogue[0] = Novice::LoadTexture("./Images/Floor2Dialog1.png");
				fairyDialogue[1] = Novice::LoadTexture("./Images/Floor2Dialog2.png");
				fairyDialogue[2] = Novice::LoadTexture("./Images/Floor2Dialog3.png");
				fairyDialogue[3] = Novice::LoadTexture("./Images/Floor2Dialog4.png");
			}
		}
		break;
	case Merchant:
		if (!isShopActive && shopCooldownTimer == 0) {
			isShopActive = true;
			Novice::PlayAudio(zhaohuSE, false, 1.0f);

			selectedShopOption = 0;
		}
		break;

	case 30:
		localPlayer.attack += 5;
		localPlayer.swordGet = true;
		swordBgm = true;
		map[currentFloor][mapY][mapX] = Empty;
		currentItemDialogIndex = 7;
		isItemDialogActive = true;
		itemDialogTimer = itemDialogDuration;
		break;
	case 31:
		localPlayer.defense += 5;
		localPlayer.shieldGet = true;
		shieldBgm = true;
		currentItemDialogIndex = 8;
		isItemDialogActive = true;
		itemDialogTimer = itemDialogDuration;
		map[currentFloor][mapY][mapX] = Empty;
		break;

	case End:
		map[currentFloor][mapY][mapX] = Empty;

		bgmClearPlayHandle = -1;
		if (killedMonsterCount >= 164) {
			currentScene = Clear; // 进入真结局

			trueEnd = true; // 可能用于显示不同的结果界面
		}
		else {

			currentScene = Clear; // 进入普通结局
			trueEnd = false;
		}

		break;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {



	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);
	LoadNumberTextures(); // 加载数字纹理

	memcpy(initialMapState, map, sizeof(map));

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };
	// 示例：绘制数字 1234 和 5678
	int clyrImage = Novice::LoadTexture("./Images/clyr.png");
	int titleImage = Novice::LoadTexture("./Images/title.png");
	int uiImage = Novice::LoadTexture("./Images/UI.png");
	int blockImage = Novice::LoadTexture("./Images/bl.png");
	int redPotionImage = Novice::LoadTexture("./Images/red_potion.png");
	int bluePotionImage = Novice::LoadTexture("./Images/blue_potion.png");
	int redKeyImage = Novice::LoadTexture("./Images/red_key.png");
	int blueKeyImage = Novice::LoadTexture("./Images/blue_key.png");
	int greenKeyImage = Novice::LoadTexture("./Images/green_key.png");
	int attackGemImage = Novice::LoadTexture("./Images/attack_gem.png");
	int defenseGemImage = Novice::LoadTexture("./Images/defense_gem.png");
	int redDoorImage = Novice::LoadTexture("./Images/red_door.png");
	int blueDoorImage = Novice::LoadTexture("./Images/blue_door.png");
	int greenDoorImage = Novice::LoadTexture("./Images/green_door.png");
	int stairsUpImage = Novice::LoadTexture("./Images/stairsUp.png");
	int stairsDownImage = Novice::LoadTexture("./Images/stairsDown.png");

	int knightImage = Novice::LoadTexture("./Images/knight.png");
	int elderlyImage = Novice::LoadTexture("./Images/elderly.png");
	int keyMerchantImage = Novice::LoadTexture("./Images/keyMerchant.png");
	int fairyImage = Novice::LoadTexture("./Images/fairy.png");

	// スプライト読み込み
	optionSprites[START_GAME] = Novice::LoadTexture("./Resources/start_game.png");
	optionSprites[GAME_INSTRUCTIONS] = Novice::LoadTexture("./Resources/instructions.png");
	optionSprites[END_GAME] = Novice::LoadTexture("./Resources/exit.png");
	modeSprites[EASY_MODE] = Novice::LoadTexture("./Resources/easy_mode.png");
	modeSprites[NORMAL_MODE] = Novice::LoadTexture("./Resources/normal_mode.png");

	shopSprites[INCREASE_ATTACK] = Novice::LoadTexture("./Resources/increase_attack.png");
	shopSprites[INCREASE_DEFENSE] = Novice::LoadTexture("./Resources/increase_defense.png");
	shopSprites[RESTORE_HEALTH] = Novice::LoadTexture("./Resources/restore_health.png");
	shopSprites[EXIT_SHOP] = Novice::LoadTexture("./Resources/exit_shop.png");
	shopMur = Novice::LoadTexture("./Resources/mur.png");
	int selectedOption = 0;
	int selectedMode = 0;

	const int frameCount = 4;
	int player_downImage[frameCount] = {
		Novice::LoadTexture("./Images/yinyang1.png"),
		Novice::LoadTexture("./Images/yinyang2.png"),
		Novice::LoadTexture("./Images/yinyang3.png"),
		Novice::LoadTexture("./Images/yinyang4.png"),
	};
	int sword_downImage[frameCount] = {
		Novice::LoadTexture("./Images/yinyang1_sword1.png"),
		Novice::LoadTexture("./Images/yinyang1_sword2.png"),
		Novice::LoadTexture("./Images/yinyang1_sword3.png"),
		Novice::LoadTexture("./Images/yinyang1_sword4.png"),
	};
	int shield_downImage[frameCount] = {
		Novice::LoadTexture("./Images/yinyang_shield1.png"),
		Novice::LoadTexture("./Images/yinyang_shield2.png"),
		Novice::LoadTexture("./Images/yinyang_shield3.png"),
		Novice::LoadTexture("./Images/yinyang_shield4.png"),
	};
	int player_upImage[frameCount] = {
		Novice::LoadTexture("./Images/yinyang_hou1.png"),
		Novice::LoadTexture("./Images/yinyang_hou2.png"),
		Novice::LoadTexture("./Images/yinyang_hou3.png"),
		Novice::LoadTexture("./Images/yinyang_hou4.png"),
	};
	int sword_upImage[frameCount] = {
		Novice::LoadTexture("./Images/yinyang_hou_sword1.png"),
		Novice::LoadTexture("./Images/yinyang_hou_sword2.png"),
		Novice::LoadTexture("./Images/yinyang_hou_sword3.png"),
		Novice::LoadTexture("./Images/yinyang_hou_sword4.png"),
	};
	int shield_upImage[frameCount] = {
		Novice::LoadTexture("./Images/yinyang_hou_shield1.png"),
		Novice::LoadTexture("./Images/yinyang_hou_shield2.png"),
		Novice::LoadTexture("./Images/yinyang_hou_shield3.png"),
		Novice::LoadTexture("./Images/yinyang_hou_shield4.png"),
	};
	int player_leftImage[frameCount] = {
		Novice::LoadTexture("./Images/yinyang_you1.png"),
		Novice::LoadTexture("./Images/yinyang_you2.png"),
		Novice::LoadTexture("./Images/yinyang_you3.png"),
		Novice::LoadTexture("./Images/yinyang_you4.png"),
	};
	int sword_leftImage[frameCount] = {
		Novice::LoadTexture("./Images/yinyang_you_sword1.png"),
		Novice::LoadTexture("./Images/yinyang_you_sword2.png"),
		Novice::LoadTexture("./Images/yinyang_you_sword3.png"),
		Novice::LoadTexture("./Images/yinyang_you_sword4.png"),
	};
	int shield_leftImage[frameCount] = {
		Novice::LoadTexture("./Images/yinyang_you_shield1.png"),
		Novice::LoadTexture("./Images/yinyang_you_shield2.png"),
		Novice::LoadTexture("./Images/yinyang_you_shield3.png"),
		Novice::LoadTexture("./Images/yinyang_you_shield4.png"),
	};
	int player_rightImage[frameCount] = {
		Novice::LoadTexture("./Images/yinyang_zuo1.png"),
		Novice::LoadTexture("./Images/yinyang_zuo2.png"),
		Novice::LoadTexture("./Images/yinyang_zuo3.png"),
		Novice::LoadTexture("./Images/yinyang_zuo4.png"),
	};
	int sword_rightImage[frameCount] = {
		Novice::LoadTexture("./Images/yinyang_zuo_sword1.png"),
		Novice::LoadTexture("./Images/yinyang_zuo_sword2.png"),
		Novice::LoadTexture("./Images/yinyang_zuo_sword3.png"),
		Novice::LoadTexture("./Images/yinyang_zuo_sword4.png"),
	};
	int shield_rightImage[frameCount] = {
		Novice::LoadTexture("./Images/yinyang_zuo_shield1.png"),
		Novice::LoadTexture("./Images/yinyang_zuo_shield2.png"),
		Novice::LoadTexture("./Images/yinyang_zuo_shield3.png"),
		Novice::LoadTexture("./Images/yinyang_zuo_shield4.png"),
	};
	int frameCounter = 0; // 动画帧计数器


	int monsterImages[][2] = {
		{Novice::LoadTexture("./Images/baybyGoblin1.png"),
		Novice::LoadTexture("./Images/baybyGoblin2.png")
		},
		{Novice::LoadTexture("./Images/goblin1.png"),
		 Novice::LoadTexture("./Images/goblin2.png")
		},
		{Novice::LoadTexture("./Images/goblin_leader1.png"),
				Novice::LoadTexture("./Images/goblin_leader2.png")}
		,

		{Novice::LoadTexture("./Images/goblin_Archer1.png"),
				Novice::LoadTexture("./Images/goblin_Archer2.png")},

		{Novice::LoadTexture("./Images/goblin_Assassin1.png"),
				Novice::LoadTexture("./Images/goblin_Assassin2.png")},

		{Novice::LoadTexture("./Images/goblin_Knight1.png"),
				Novice::LoadTexture("./Images/goblin_Knight2.png")},

		{Novice::LoadTexture("./Images/goblin_King1.png"),
				Novice::LoadTexture("./Images/goblin_King2.png")},

		{Novice::LoadTexture("./Images/goblin_Hero1.png"),
				Novice::LoadTexture("./Images/goblin_Hero2.png")},


	};


	int bloodImage = Novice::LoadTexture("./Images/blood.png");
	int yanImage = Novice::LoadTexture("./Images/yan.png");
	int yan2Image = Novice::LoadTexture("./Images/yan2.png");

	int floorImage = Novice::LoadTexture("./Resources/floor.png");
	int swordImage = Novice::LoadTexture("./Resources/sword.png");
	int shieldImage = Novice::LoadTexture("./Resources/shield.png");
	int HelpImge = Novice::LoadTexture("./Images/help.png");
	int modeImga = Novice::LoadTexture("./Images/mode.png");

	int normalEndingImage = Novice::LoadTexture("./Resources/nend.png");
	int trueEndingImage = Novice::LoadTexture("./Resources/tend.png");
	int deadEndingImage = Novice::LoadTexture("./Resources/dend.png");

	//对话框
	itemDialogImage[0] = Novice::LoadTexture("./Images/RedPotionDialog.png");
	itemDialogImage[1] = Novice::LoadTexture("./Images/BluePotionDialog.png");
	itemDialogImage[2] = Novice::LoadTexture("./Images/AttackGemDialog.png");
	itemDialogImage[3] = Novice::LoadTexture("./Images/DefenseGemDialog.png");
	itemDialogImage[4] = Novice::LoadTexture("./Images/green_keyDialog.png");
	itemDialogImage[5] = Novice::LoadTexture("./Images/blue_keyDialog.png");
	itemDialogImage[6] = Novice::LoadTexture("./Images/red_keyDialog.png");
	itemDialogImage[7] = Novice::LoadTexture("./Images/ken.png");
	itemDialogImage[8] = Novice::LoadTexture("./Images/dun.png");

	goldDialogImage = Novice::LoadTexture("./Images/GoldDialog.png");

	fairyDialogue[0] = Novice::LoadTexture("./Images/RedPotionDialog.png");
	fairyDialogue[1] = Novice::LoadTexture("./Images/green_keyDialog.png");
	fairyDialogue[2] = Novice::LoadTexture("./Images/blue_keyDialog.png");
	fairyDialogue[3] = Novice::LoadTexture("./Images/red_keyDialog.png");

	int playerSpeed = 4; ///プレイヤーの速度

	int ScrollX = 0;
	int ScrollY = 0;


	//BGM
	int bgmTitle, bgmDifficultySelect, bgmGame, bgmGetSwordShield, bgmClear, bgmBoss;

	bgmTitle = Novice::LoadAudio("./Sounds/title_bgm.wav");
	bgmDifficultySelect = Novice::LoadAudio("./Sounds/difficulty_bgm.wav");
	bgmGame = Novice::LoadAudio("./Sounds/game_bgm.wav");
	bgmGetSwordShield = Novice::LoadAudio("./Sounds/get_sword_shield.wav");
	bgmClear = Novice::LoadAudio("./Sounds/clear_bgm.wav");
	bgmBoss = Novice::LoadAudio("./Sounds/boss_bgm.wav");


	//se
	int titleSe = Novice::LoadAudio("./Sounds/titleSe.wav");
	int strikeSe = Novice::LoadAudio("./Sounds/strike02.wav");
	int punchSe = Novice::LoadAudio("./Sounds/21.wav");
	int jiaoyiSe = Novice::LoadAudio("./Sounds/Z.wav");


	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		//BGM
		if (!Novice::IsPlayingAudio(bgmTitle) && bgmTitlePlayHandle == -1)
		{
			Novice::StopAudio(bgmClearPlayHandle);
			bgmClearPlayHandle = false;

			bgmTitlePlayHandle = Novice::PlayAudio(bgmTitle, true, 1);
		}
		if (!Novice::IsPlayingAudio(bgmDifficultySelect) && bgmDifficultySelectPlayHandle == -1)
		{
			Novice::StopAudio(bgmTitlePlayHandle);
			bgmTitlePlayHandle = false;

			bgmDifficultySelectPlayHandle = Novice::PlayAudio(bgmDifficultySelect, true, 1);
		}
		if (!Novice::IsPlayingAudio(bgmGame) && bgmGamePlayHandle == -1)
		{
			Novice::StopAudio(bgmDifficultySelectPlayHandle);
			bgmDifficultySelectPlayHandle = false;

			bgmGamePlayHandle = Novice::PlayAudio(bgmGame, true, 1);
		}
		if (!Novice::IsPlayingAudio(bgmGetSwordShield) && bgmGetSwordShieldPlayHandle == -1)
		{
			Novice::StopAudio(bgmGamePlayHandle);
			bgmGamePlayHandle = false;

			bgmGetSwordShieldPlayHandle = Novice::PlayAudio(bgmGetSwordShield, true, 1);
		}
		if (!Novice::IsPlayingAudio(bgmBoss) && bgmBossPlayHandle == -1)
		{
			Novice::StopAudio(bgmGamePlayHandle);
			bgmGamePlayHandle = false;
			Novice::StopAudio(bgmGetSwordShieldPlayHandle);
			bgmGetSwordShieldPlayHandle = false;

			bgmBossPlayHandle = Novice::PlayAudio(bgmBoss, true, 1);
		}
		if (!Novice::IsPlayingAudio(bgmClear) && bgmClearPlayHandle == -1)
		{
			Novice::StopAudio(bgmGetSwordShieldPlayHandle);
			bgmGetSwordShieldPlayHandle = false;

			Novice::StopAudio(bgmGamePlayHandle);
			bgmGamePlayHandle = false;

			Novice::StopAudio(bgmBossPlayHandle);
			bgmBossPlayHandle = false;

			bgmClearPlayHandle = Novice::PlayAudio(bgmClear, true, 1);
		}
		//ジャンプ
	// 玩家初始参数


		///
		/// ↓更新処理ここから
		///
		switch (currentScene) {
		case Title: {
			Novice::DrawSprite(0, 0, titleImage, 1.0f, 1.0f, 0.0f, WHITE);

			// 上下キーで選択肢を移動
			if (preKeys[DIK_UP] == 0 && keys[DIK_UP] != 0 || preKeys[DIK_W] == 0 && keys[DIK_W] != 0) {
				selectedOption = (selectedOption - 1 + MAX_OPTIONS) % MAX_OPTIONS;
				Novice::PlayAudio(titleSe, false, 1.0f);

			}
			if (preKeys[DIK_DOWN] == 0 && keys[DIK_DOWN] != 0 || preKeys[DIK_S] == 0 && keys[DIK_S] != 0) {
				selectedOption = (selectedOption + 1) % MAX_OPTIONS;
				Novice::PlayAudio(titleSe, false, 1.0f);

			}
			// 透明度を更新
			for (int i = 0; i < MAX_OPTIONS; ++i) {
				transparency[i] = (i == selectedOption) ? 255 : 128;
			}
			// Enterキーで選択を確定
			if (preKeys[DIK_RETURN] == 0 && keys[DIK_RETURN] != 0 || preKeys[DIK_SPACE] == 0 && keys[DIK_SPACE] != 0) {
				switch (selectedOption) {
				case START_GAME:
					currentScene = MODE_SELECTION;
					InitializeGame(); // 初始化游戏
					bgmDifficultySelectPlayHandle = -1;
					break;
				case GAME_INSTRUCTIONS:
					// ゲーム説明画面への遷移（省略可）
					currentScene = Help;

					break;
				case END_GAME:
					// アプリケーション終了
					Novice::Finalize();
					return 0;
				}
			}
			for (int i = 0; i < MAX_OPTIONS; ++i) {
				Novice::DrawSprite(options[i].x, options[i].y, optionSprites[i], 1.0f, 1.0f, 0.0f, transparency[i]);
			}
			break;
		}
		case MODE_SELECTION: {


			// 上下キーでモード選択
			if (preKeys[DIK_UP] == 0 && keys[DIK_UP] != 0 || preKeys[DIK_W] == 0 && keys[DIK_W] != 0) {
				selectedMode = (selectedMode - 1 + MAX_MODES) % MAX_MODES;
				Novice::PlayAudio(titleSe, false, 1.0f);

			}
			if (preKeys[DIK_DOWN] == 0 && keys[DIK_DOWN] != 0 || preKeys[DIK_S] == 0 && keys[DIK_S] != 0) {
				selectedMode = (selectedMode + 1) % MAX_MODES;
				Novice::PlayAudio(titleSe, false, 1.0f);

			}
			// 透明度を更新
			for (int i = 0; i < MAX_MODES; ++i) {
				modeTransparency[i] = (i == selectedMode) ? 255 : 128;
			}
			// Enterキーでモードを確定
			if (preKeys[DIK_RETURN] == 0 && keys[DIK_RETURN] != 0 || preKeys[DIK_SPACE] == 0 && keys[DIK_SPACE] != 0) {
				currentScene = Game;
				// 選択したモードによる初期化処理（省略可）
				if (selectedMode == EASY_MODE) {
					bgmGamePlayHandle = -1;
					localPlayer = {
						  kBlockSize + kPlayerRadius * 11, kBlockSize + kPlayerRadius * 23,
						  500,  // 初始生命值
						  10,    // 初始攻击力
						  5,    // 初始防御力
						  5, 5, 5,  // 钥匙数量
						  50     // 初始金币数量
					};
					// 簡単モードの初期化処理（省略可）
				}
				else if (selectedMode == NORMAL_MODE) {
					bgmGamePlayHandle = -1;

					localPlayer = {
						  kBlockSize + kPlayerRadius * 11, kBlockSize + kPlayerRadius * 23,
						  100,  // 初始生命值
						  5,    // 初始攻击力
						  5,    // 初始防御力
						  0, 0, 0,  // 钥匙数量
						  0     // 初始金币数量
					};
				}
			}
			Novice::DrawSprite(0, 0, modeImga, 1.0f, 1.0f, 0.0f, WHITE);

			for (int i = 0; i < MAX_MODES; ++i) {
				Novice::DrawSprite(modeOptions[i].x, modeOptions[i].y, modeSprites[i], 1.0f, 1.0f, 0.0f, modeTransparency[i]);
			}

			// 選択肢の三角形（矢印）
			int arrowX = modeOptions[selectedMode].x - 50;
			int arrowY = modeOptions[selectedMode].y;
			Novice::DrawSprite(arrowX, arrowY, arrowSprite, 1.0f, 1.0f, 0.0f, 255);


			break;
		}
		case Help:

			Novice::DrawSprite(0, 0, HelpImge, 1.0f, 1.0f, 0.0f, WHITE);
			if (preKeys[DIK_RETURN] == 0 && keys[DIK_RETURN] != 0 || preKeys[DIK_SPACE] == 0 && keys[DIK_SPACE] != 0) {
				currentScene = Title;
			}
			break;
		case Game: {

			if (enemyCount <= 60) {
				enemyFrame = 0;
			}
			else {
				enemyFrame = 1;
			}

			enemyCount++;

			if (enemyCount >= 120) {
				enemyCount = 0;
			}



			// 初始BGM播放
			if (killedMonsterCount >= 164 && !hasPlayedBgm) {
				// 在地图上选择一个位置，假设为当前楼层的(6, 6)
				int bossX = 6;
				int bossY = 10;
				// 更新地图
				map[7][bossY][bossX] = GoblinHero;
				bossBgm = true;
				hasPlayedBgm = true;  // 标记BGM已经播放过
			}

			if (killedMonsterCount >= 164) {
				trueEnd = true;
			}

			if (bossBgm) {
				// 播放boss BGM（假设 bgmBossPlayHandle 会开始播放 BGM）
				bgmBossPlayHandle = -1;

				// 播放后立即关闭 bossBgm
				bossBgm = false;
			}

			// 捡了剑盾后切换BGM，确保在未播放BOSS BGM时才允许切换
			if (shieldBgm && swordBgm && !bossBgm && !hasPlayedBgm) {
				swordBgm = false;
				shieldBgm = false;
				bgmGetSwordShieldPlayHandle = -1;
			}





			if (isSceneTransition) {
				transitionTimer++;

				if (isTransitionFadeOut && transitionTimer >= transitionDuration) {
					// 渐暗结束，切换楼层
					isTransitionFadeOut = false;
					transitionTimer = 0;

					// 切换楼层逻辑
					if (map[currentFloor][localPlayer.posY / kBlockSize][localPlayer.posX / kBlockSize] == stairsUp) {
						currentFloor++;
					}
					else if (map[currentFloor][localPlayer.posY / kBlockSize][localPlayer.posX / kBlockSize] == stairsDown) {
						currentFloor--;
					}
				}

				if (!isTransitionFadeOut && transitionTimer >= transitionDuration) {
					// 渐亮结束，场景切换完成
					isSceneTransition = false;
				}
			}






			bool isMoving = false;

			// 玩家向上移动
			if (!inBattle && !isFairyDialogueActive && !isShopActive && !isSceneTransition)
				// 玩家移动逻辑

			{// 游戏场景逻辑
				if (keys[DIK_Z] && preKeys[DIK_Z] == 0) {
					currentScene = MonsterManual; // 切换到怪物手册场景
				}
				if (localPlayer.health <= 0) {
					currentScene = GameOver; // 玩家死亡切换到游戏结束场景
				}
				if (keys[DIK_W] &&
					CheckCollision(localPlayer.posX - kPlayerRadius, localPlayer.posY - kPlayerRadius - playerSpeed) &&
					CheckCollision(localPlayer.posX + kPlayerRadius - 1, localPlayer.posY - kPlayerRadius - playerSpeed)
					|| keys[DIK_UP] &&
					CheckCollision(localPlayer.posX - kPlayerRadius, localPlayer.posY - kPlayerRadius - playerSpeed) &&
					CheckCollision(localPlayer.posX + kPlayerRadius - 1, localPlayer.posY - kPlayerRadius - playerSpeed)) {
					localPlayer.posY -= playerSpeed;
					localPlayer.direction = 1;
					isMoving = true;


				}
				// 玩家向下移动
				if (keys[DIK_S] &&
					CheckCollision(localPlayer.posX - kPlayerRadius, localPlayer.posY + kPlayerRadius + playerSpeed - 1)
					&& CheckCollision(localPlayer.posX + kPlayerRadius - 1, localPlayer.posY + kPlayerRadius + playerSpeed - 1)
					|| keys[DIK_DOWN] &&
					CheckCollision(localPlayer.posX - kPlayerRadius, localPlayer.posY + kPlayerRadius + playerSpeed - 1) &&
					CheckCollision(localPlayer.posX + kPlayerRadius - 1, localPlayer.posY + kPlayerRadius + playerSpeed - 1)) {
					localPlayer.posY += playerSpeed;
					localPlayer.direction = 0;
					isMoving = true;


				}
				// 玩家向左移动
				if (keys[DIK_A] &&
					CheckCollision(localPlayer.posX - kPlayerRadius - playerSpeed, localPlayer.posY - kPlayerRadius) &&
					CheckCollision(localPlayer.posX - kPlayerRadius - playerSpeed, localPlayer.posY + kPlayerRadius - 1)
					|| keys[DIK_LEFT] &&
					CheckCollision(localPlayer.posX - kPlayerRadius - playerSpeed, localPlayer.posY - kPlayerRadius) &&
					CheckCollision(localPlayer.posX - kPlayerRadius - playerSpeed, localPlayer.posY + kPlayerRadius - 1)) {
					localPlayer.posX -= playerSpeed;
					localPlayer.direction = 2;
					isMoving = true;


				}
				// 玩家向右移动
				if (keys[DIK_D] &&
					CheckCollision(localPlayer.posX + kPlayerRadius + playerSpeed - 1, localPlayer.posY - kPlayerRadius) &&
					CheckCollision(localPlayer.posX + kPlayerRadius + playerSpeed - 1, localPlayer.posY + kPlayerRadius - 1)
					|| keys[DIK_RIGHT] &&
					CheckCollision(localPlayer.posX + kPlayerRadius + playerSpeed - 1, localPlayer.posY - kPlayerRadius) &&
					CheckCollision(localPlayer.posX + kPlayerRadius + playerSpeed - 1, localPlayer.posY + kPlayerRadius - 1)) {
					localPlayer.posX += playerSpeed;
					localPlayer.direction = 3;
					isMoving = true;


				}
				InteractWithChip(localPlayer.posX, localPlayer.posY);

			}
			// 计算动画帧
			if (isMoving) {
				frameCounter++;
				if (frameCounter >= 10) { // 每 10 帧切换一次动画
					localPlayer.frame = (localPlayer.frame + 1) % frameCount;
					frameCounter = 0;
				}
			}
			else {
				localPlayer.frame = 0; // 停止移动时重置动画
			}




			///
			/// ↑更新処理ここまで
			///

			///
			/// ↓描画処理ここから
			///
			// 地图绘制部分
			Novice::DrawSprite(-255, 0, uiImage, 1, 1, 0, WHITE);

			for (int y = 0; y < kBlockHeight; y++) {
				for (int x = 0; x < KBlockwidth; x++) {
					int mapChip = map[currentFloor][y][x];
					int drawX = x * kBlockSize - ScrollX;
					int drawY = y * kBlockSize - ScrollY;

					if (drawX + kBlockSize > 0 && drawX < kWindowWidth &&
						drawY + kBlockSize > 0 && drawY < kWindowHeight) {
						switch (mapChip) {
						case Brick:

							Novice::DrawSprite(drawX, drawY, blockImage, 1, 1, 0, WHITE);
							break;
						case Monsters:


							break;
						case RedPotion:

							Novice::DrawSprite(drawX, drawY, redPotionImage, 2.0f, 2.0f, 0, WHITE);
							break;
						case BluePotion:

							Novice::DrawSprite(drawX, drawY, bluePotionImage, 2.0f, 2.0f, 0, WHITE);
							break;
						case AttackGem:

							Novice::DrawSprite(drawX, drawY, attackGemImage, 2.0f, 2.0f, 0, WHITE);
							break;
						case DefenseGem:
							Novice::DrawSprite(drawX, drawY, defenseGemImage, 2.0f, 2.0f, 0, WHITE);
							break;
						case RedKey:
							Novice::DrawSprite(drawX, drawY, redKeyImage, 0.5f, 0.5f, 0, WHITE);
							break;
						case BlueKey:
							Novice::DrawSprite(drawX, drawY, blueKeyImage, 0.5f, 0.5f, 0, WHITE);
							break;
						case GreenKey:
							Novice::DrawSprite(drawX, drawY, greenKeyImage, 0.5f, 0.5f, 0, WHITE);
							break;
						case RedDoor:
							Novice::DrawSprite(drawX, drawY, redDoorImage, 0.5f, 0.5f, 0, WHITE);
							break;
						case BlueDoor:
							Novice::DrawSprite(drawX, drawY, blueDoorImage, 0.5f, 0.5f, 0, WHITE);
							break;
						case GreenDoor:
							Novice::DrawSprite(drawX, drawY, greenDoorImage, 0.5f, 0.5f, 0, WHITE);
							break;
						case stairsUp:
							Novice::DrawSprite(drawX, drawY, stairsUpImage, 2.0f, 2.0f, 0, 0xFFFFFFAA);
							break;
						case stairsDown:
							Novice::DrawSprite(drawX, drawY, stairsDownImage, 2.0f, 2.0f, 0, 0xFFFFFFAA);
							break;
						case blood:
							Novice::DrawSprite(drawX, drawY, bloodImage, 1, 1, 0, 0xFFFFFFAA);
							break;
						case BabyGoblin:
							Novice::DrawSprite(drawX, drawY, monsterImages[0][enemyFrame], 0.5f, 0.5f, 0, WHITE);
							break;
						case Goblin:
							Novice::DrawSprite(drawX, drawY, monsterImages[1][enemyFrame], 0.5f, 0.5f, 0, WHITE);
							break;
						case GoblinLeader:
							Novice::DrawSprite(drawX, drawY, monsterImages[2][enemyFrame], 0.5f, 0.5f, 0, WHITE);
							break;
						case GoblinArcher:
							Novice::DrawSprite(drawX, drawY, monsterImages[3][enemyFrame], 0.5f, 0.5f, 0, WHITE);
							break;
						case GoblinAssassin:
							Novice::DrawSprite(drawX, drawY, monsterImages[4][enemyFrame], 0.5f, 0.5f, 0, WHITE);
							break;
						case GoblinKnight:
							Novice::DrawSprite(drawX, drawY, monsterImages[5][enemyFrame], 0.5f, 0.5f, 0, WHITE);
							break;
						case GoblinKing:
							Novice::DrawSprite(drawX, drawY, monsterImages[6][enemyFrame], 0.5f, 0.5f, 0, WHITE);
							break;
						case Merchant:
							Novice::DrawSprite(drawX, drawY, knightImage, 1, 1, 0, WHITE);
							break;
						case Elderly:
							Novice::DrawSprite(drawX, drawY, elderlyImage, 0.5f, 0.5f, 0, WHITE);
							break;
						case KeyMerchant:
							Novice::DrawSprite(drawX, drawY, keyMerchantImage, 0.5f, 0.5f, 0, WHITE);
							break;
						case Fairy:
							Novice::DrawSprite(drawX, drawY, fairyImage, 1, 1, 0, WHITE);
							break;
						case IronSword:
							Novice::DrawSprite(drawX, drawY, swordImage, 1, 1, 0, WHITE);
							break;
						case IronShield:
							Novice::DrawSprite(drawX, drawY, shieldImage, 1, 1, 0, WHITE);
							break;
						case 37:
							Novice::DrawSprite(drawX, drawY, clyrImage, 2, 2, 0, WHITE);
							break;
						}

					}
				}
			}
			for (int y = 0; y < kBlockHeight; y++) {
				for (int x = 0; x < KBlockwidth; x++) {
					int mapChip = map[currentFloor][y][x];
					int drawX = x * kBlockSize - ScrollX;
					int drawY = y * kBlockSize - ScrollY;

					switch (mapChip) {
					case BabyGoblin:
					case Goblin:
					case GoblinLeader:
					case GoblinArcher:
					case GoblinAssassin:
					case GoblinKnight:
					case GoblinKing:
					case GoblinHero:
					{
						int monsterIndex = mapChip - BabyGoblin; // 计算怪物索引
						Monster& monster = monsterManual[monsterIndex];

						// 计算玩家受到的总伤害
						int damageToMonsterPerHit = localPlayer.attack - monster.defense;
						if (damageToMonsterPerHit < 1) damageToMonsterPerHit = 1;

						int hitsToDefeatMonster = (monster.health + damageToMonsterPerHit - 1) / damageToMonsterPerHit;
						int damageToPlayerPerHit = monster.attack - localPlayer.defense;
						if (damageToPlayerPerHit < 1) damageToPlayerPerHit = 0;

						int totalDamageToPlayer = damageToPlayerPerHit * (hitsToDefeatMonster - 1);

						// 绘制怪物
						Novice::DrawSprite(drawX, drawY, monsterImages[monsterIndex][enemyFrame], 0.5f, 0.5f, 0, WHITE);

						// 在怪物头上显示 totalDamageToPlayer
						DrawNumber(totalDamageToPlayer, drawX + 2, drawY, 15, 15, WHITE); // 20x20，红色字体
					}
					break;
					}
				}
			}


			// プレイヤの画像
			int* currentAnimation = nullptr;
			int* swordAnimation = nullptr;
			int* shieldAnimation = nullptr;


			switch (localPlayer.direction) {
			case 0:
				currentAnimation = player_downImage;
				swordAnimation = sword_downImage;
				shieldAnimation = shield_downImage;
				break;
			case 1:
				currentAnimation = player_upImage;
				swordAnimation = sword_upImage;
				shieldAnimation = shield_upImage;
				break;
			case 2:
				currentAnimation = player_leftImage;
				swordAnimation = sword_leftImage;
				shieldAnimation = shield_leftImage;
				break;
			case 3:
				currentAnimation = player_rightImage;
				swordAnimation = sword_rightImage;
				shieldAnimation = shield_rightImage;
				break;
			}

			if (currentAnimation) {
				Novice::DrawSprite(localPlayer.posX - kPlayerRadius, localPlayer.posY - kPlayerRadius, currentAnimation[localPlayer.frame], 1, 1, 0, WHITE);
				if (localPlayer.swordGet)
				{
					Novice::DrawSprite(localPlayer.posX - kPlayerRadius, localPlayer.posY - kPlayerRadius, swordAnimation[localPlayer.frame], 1, 1, 0, WHITE);

				}
				if (localPlayer.shieldGet) {
					Novice::DrawSprite(localPlayer.posX - kPlayerRadius, localPlayer.posY - kPlayerRadius, shieldAnimation[localPlayer.frame], 1, 1, 0, WHITE);

				}
			}



			Novice::DrawSprite(kWindowWidth - 270, 550, yanImage, 1, 1, 0, WHITE);

			DrawNumber(localPlayer.health, kWindowWidth - 230, 227, 25, 25, WHITE); // 绘制生命值
			DrawNumber(localPlayer.attack, kWindowWidth - 230, 293, 25, 25, WHITE); // 绘制攻击力
			DrawNumber(localPlayer.defense, kWindowWidth - 230, 357, 25, 25, WHITE); // 绘制防御力
			DrawNumber(localPlayer.gold, kWindowWidth - 230, 422, 25, 25, WHITE); // 绘制金币数量

			// 绘制钥匙UI
			int baseX = kWindowWidth - 200;  // 右下角X起始坐标
			int baseY = kWindowHeight - 100; // 右下角Y起始坐标
			int iconSize = 50;               // 图标大小
			int spacing = 10;                // 图标间距

			// 绘制红钥匙
			Novice::DrawSprite(baseX, baseY, redKeyImage, 0.5f, 0.5f, 0.0f, WHITE);
			DrawNumber(localPlayer.redKeys, baseX + iconSize + spacing, baseY + iconSize / 4, 25, 25, WHITE);

			// 绘制蓝钥匙
			Novice::DrawSprite(baseX, baseY - iconSize - spacing, blueKeyImage, 0.5f, 0.5f, 0.0f, WHITE);
			DrawNumber(localPlayer.blueKeys, baseX + iconSize + spacing, baseY - iconSize - spacing + iconSize / 4, 25, 25, WHITE);

			// 绘制绿钥匙
			Novice::DrawSprite(baseX, baseY - 2 * (iconSize + spacing), greenKeyImage, 0.5f, 0.5f, 0.0f, WHITE);
			DrawNumber(localPlayer.greenKeys, baseX + iconSize + spacing, baseY - 2 * (iconSize + spacing) + iconSize / 4, 25, 25, WHITE);

			Novice::DrawSprite(kWindowWidth - 150, 80, floorImage, 1, 1, 0.0f, WHITE);
			DrawNumber(currentFloor + 1, kWindowWidth - 270, 87, 46, 46, WHITE);

			if (isSceneTransition) {
				// 计算动画进度
				float progress = static_cast<float>(transitionTimer) / transitionDuration; // 从 0 到 1 的比例

				// 如果是渐亮效果，反转进度
				if (!isTransitionFadeOut) {
					progress = 1.0f - progress;
				}

				// 动态计算上下部分的 Y 坐标
				int Y1 = static_cast<int>(-kWindowHeight / 2 * progress); // 上部分向上移动
				int Y2 = static_cast<int>(kWindowHeight / 2 + kWindowHeight / 2 * progress); // 下部分向下移动

				// 绘制上下两个矩形
				Novice::DrawBox(0, Y1, kWindowWidth, kWindowHeight / 2, 0, BLACK, kFillModeSolid);
				Novice::DrawBox(0, Y2, kWindowWidth, kWindowHeight / 2, 0, BLACK, kFillModeSolid);

				// 更新计时器
				transitionTimer++;
				if (transitionTimer >= transitionDuration) {
					if (isTransitionFadeOut) {
						// 完成场景离开
						isTransitionFadeOut = false;
						transitionTimer = 0;

						// 触发场景切换逻辑
						// (根据你的游戏逻辑切换楼层或场景)
					}
					else {
						// 完成场景进入
						isSceneTransition = false;
					}
				}
			}
			if (isGoldDialogActive) {
				int dialogX = kWindowWidth / 2 - 400; // 图片居中
				int dialogY = kWindowHeight / 2 - 50;
				Novice::DrawBox(0, dialogY - 50, kWindowWidth - 300, 150, 0, 0x000000aa, kFillModeSolid);
				Novice::DrawSprite(dialogX, dialogY, goldDialogImage, 1.0f, 1.0f, 0.0f, WHITE);

				// 计时器逻辑
				if (goldDialogTimer > 0) {
					goldDialogTimer--;
				}
				else {
					isGoldDialogActive = false;
				}
			}

			if (fairyDialogueCooldownTimer > 0) {
				fairyDialogueCooldownTimer--; // 冷却计时递减
			}
			if (isItemDialogActive && currentItemDialogIndex >= 0 && currentItemDialogIndex < kItemDialogCount) {
				int dialogX = kWindowWidth / 2 - 400; // 图片居中
				int dialogY = kWindowHeight / 2 - 50;
				Novice::DrawBox(0, dialogY - 50, kWindowWidth - 300, 150, 0, 0x000000aa, kFillModeSolid);
				Novice::DrawSprite(dialogX, dialogY, itemDialogImage[currentItemDialogIndex], 1.0f, 1.0f, 0.0f, WHITE);

				// 计时器逻辑
				if (itemDialogTimer > 0) {
					itemDialogTimer--;
				}
				else {
					isItemDialogActive = false;
					currentItemDialogIndex = -1; // 重置对话框索引
				}
			}

			//会話処理			////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if (fairyDialogueCooldownTimer > 0) {
				fairyDialogueCooldownTimer--; // 冷却计时递减
			}
			if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0 || keys[DIK_RETURN] && preKeys[DIK_RETURN] == 0) {
				currentFairyDialogueIndex++;
				if (currentFairyDialogueIndex >= dialogueCount) {
					isFairyDialogueActive = false; // 退出对话模式
					fairyDialogueCooldownTimer = fairyDialogueCooldownDuration; // 设置冷却时间
					currentFairyDialogueIndex = 0; // 重置对话索引
				}
			}
			if (isFairyDialogueActive) {
				// 显示对话框
				int dialogueX = 0;
				int dialogueY = kWindowHeight - 176;
				//Novice::DrawBox(0, dialogueY - 50, kWindowWidth - 300, 150, 0, BLACK, kFillModeSolid);
				Novice::DrawSprite(dialogueX, dialogueY, fairyDialogue[currentFairyDialogueIndex], 1.0f, 1.0f, 0.0f, WHITE);

			}
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if (shopCooldownTimer > 0) {
				shopCooldownTimer--; // 冷却时间递减
			}
			if (isShopActive) {

				// 上下键选择选项
				if (preKeys[DIK_UP] == 0 && keys[DIK_UP] != 0) {
					selectedShopOption = (selectedShopOption - 1 + MAX_SHOP_OPTIONS) % MAX_SHOP_OPTIONS;
				}
				if (preKeys[DIK_DOWN] == 0 && keys[DIK_DOWN] != 0) {
					selectedShopOption = (selectedShopOption + 1) % MAX_SHOP_OPTIONS;
				}

				// 更新透明度
				for (int i = 0; i < MAX_SHOP_OPTIONS; ++i) {
					shopTransparency[i] = (i == selectedShopOption) ? 255 : 128;
				}

				if (preKeys[DIK_RETURN] == 0 && keys[DIK_RETURN] != 0) {
					switch (selectedShopOption) {
					case INCREASE_ATTACK:
						if (localPlayer.gold >= shopOptionPrices[INCREASE_ATTACK]) {
							localPlayer.gold -= shopOptionPrices[INCREASE_ATTACK];
							localPlayer.attack += 3;
							Novice::PlayAudio(jiaoyiSe, false, 1.0f);
							shopOptionPrices[INCREASE_ATTACK] += priceIncrement;
							shopOptionPrices[INCREASE_DEFENSE] += priceIncrement;
							shopOptionPrices[RESTORE_HEALTH] += priceIncrement;

						}
						else
						{
							isShopActive = false;
							shopCooldownTimer = shopCooldownDuration; // 冷却时间
						}
						break;
					case INCREASE_DEFENSE:
						if (localPlayer.gold >= shopOptionPrices[INCREASE_DEFENSE]) {
							localPlayer.gold -= shopOptionPrices[INCREASE_DEFENSE];
							localPlayer.defense += 3;
							Novice::PlayAudio(jiaoyiSe, false, 1.0f);
							shopOptionPrices[INCREASE_ATTACK] += priceIncrement;
							shopOptionPrices[INCREASE_DEFENSE] += priceIncrement;
							shopOptionPrices[RESTORE_HEALTH] += priceIncrement;
						}
						else
						{
							isShopActive = false;
							shopCooldownTimer = shopCooldownDuration; // 冷却时间
						}
						break;
					case RESTORE_HEALTH:
						if (localPlayer.gold >= shopOptionPrices[RESTORE_HEALTH]) {
							localPlayer.gold -= shopOptionPrices[RESTORE_HEALTH];
							localPlayer.health += 300;
							Novice::PlayAudio(jiaoyiSe, false, 1.0f);
							shopOptionPrices[INCREASE_ATTACK] += priceIncrement;
							shopOptionPrices[INCREASE_DEFENSE] += priceIncrement;
							shopOptionPrices[RESTORE_HEALTH] += priceIncrement;
						}
						else
						{
							isShopActive = false;
							shopCooldownTimer = shopCooldownDuration; // 冷却时间
						}
						break;
					case EXIT_SHOP:
						isShopActive = false;
						shopCooldownTimer = shopCooldownDuration; // 冷却时间
						break;
					}
				}

				// 绘制商店菜单
				DrawShopMenu();
				for (int i = 0; i < MAX_SHOP_OPTIONS; ++i) {
					DrawNumber(shopOptionPrices[INCREASE_DEFENSE], 264, 675, 30, 30, 0xffff00ff); // 绘制生命值
				}
			}
			else if (shopCooldownTimer > 0) {
				shopCooldownTimer--; // 冷却时间减少
			}


			//战斗逻辑
			if (inBattle && currentEnemy) {

				// 绘制背景
				Novice::DrawBox(0, 0, kWindowWidth, kWindowHeight, 0, 0x000000dd, kFillModeSolid);

				// 绘制主角
				Novice::DrawSprite(kWindowWidth / 6, kWindowHeight / 2, player_downImage[0], 4.0f, 4.0f, 0,
					inBattle && battleFrameCounter % 20 < 10 ? RED : WHITE);
				if (localPlayer.swordGet)
				{
					Novice::DrawSprite(kWindowWidth / 6, kWindowHeight / 2, sword_downImage[0], 4, 4, 0, WHITE);

				}
				if (localPlayer.shieldGet) {
					Novice::DrawSprite(kWindowWidth / 6, kWindowHeight / 2, shield_downImage[0], 4, 4, 0, WHITE);

				}

				// 确保 currentEnemy 有效并计算其索引
				if (currentEnemy == nullptr) {
					break; // 若敌人无效，安全退出相关逻辑
				}
				if (currentEnemy) {
					int monsterIndex = static_cast<int>(currentEnemy - &monsterManual[0]);

					// 检查索引范围，避免越界
					if (monsterIndex >= 0 && monsterIndex < sizeof(monsterImages) / sizeof(monsterImages[0])) {
						Novice::DrawSprite(2 * kWindowWidth / 3, kWindowHeight / 2, monsterImages[monsterIndex][0], 2.0f, 2.0f, 0,
							inBattle && battleFrameCounter % 20 < 10 ? RED : WHITE);
					}
				}

				// 显示状态信息
				DrawNumber(localPlayer.health, kWindowWidth / 4 - 50, kWindowHeight / 2 - 250, 25, 25, WHITE); // 主角血量
				DrawNumber(localPlayer.attack, kWindowWidth / 4 - 50, kWindowHeight / 2 - 200, 25, 25, WHITE); // 主角血量
				DrawNumber(localPlayer.defense, kWindowWidth / 4 - 50, kWindowHeight / 2 - 150, 25, 25, WHITE); // 主角血量

				// 在主界面显示金币数量（例如绘制在屏幕右上角）
				DrawNumber(currentEnemy->health, 3 * kWindowWidth / 4 - 50, kWindowHeight / 2 - 250, 25, 25, WHITE); // 怪物血量
				DrawNumber(currentEnemy->attack, 3 * kWindowWidth / 4 - 50, kWindowHeight / 2 - 200, 25, 25, WHITE); // 怪物血量
				DrawNumber(currentEnemy->defense, 3 * kWindowWidth / 4 - 50, kWindowHeight / 2 - 150, 25, 25, WHITE); // 怪物血量




				battleFrameCounter++;

				// 每30帧攻击一次
				if (battleFrameCounter % 20 == 0) {
					// 玩家攻击敌人
					int damageToEnemy = localPlayer.attack - currentEnemy->defense;
					if (damageToEnemy < 1) damageToEnemy = 1;
					currentEnemy->health -= damageToEnemy;

					if (localPlayer.swordGet) {
						Novice::PlayAudio(strikeSe, false, 1.0f);
					}
					else
					{
						Novice::PlayAudio(punchSe, false, 1.0f);

					}
					DrawNumber(damageToEnemy, 2 * kWindowWidth / 3 + 50, kWindowHeight / 2 - 50, 64, 64, RED); // 主角血量

					if (currentEnemy->health <= 0) {
						map[currentFloor][localPlayer.posY / kBlockSize][localPlayer.posX / kBlockSize] = blood;
						killedMonsterCount++;   // 增加击杀计数
						localPlayer.gold += currentEnemy->gold; // 增加玩家金币

						// 设置金币对话框状态
						earnedGold = currentEnemy->gold;   // 记录获得的金币
						isGoldDialogActive = true;         // 激活金币对话框
						goldDialogTimer = goldDialogDuration;

						currentEnemy->health = currentEnemy->initialHealth;  // 恢复血量

						inBattle = false; // 退出战斗模式
						currentEnemy = nullptr;
					}
					else {
						// 敌人攻击玩家
						int damageToPlayer = currentEnemy->attack - localPlayer.defense;
						if (damageToPlayer < 1) damageToPlayer = 0;
						localPlayer.health -= damageToPlayer;
						DrawNumber(damageToPlayer, kWindowWidth / 6 + 50, kWindowHeight / 2 - 50, 64, 64, RED);

						if (localPlayer.health <= 0) {
							inBattle = false; // 退出战斗模式
							currentEnemy = nullptr;
							bgmClearPlayHandle = -1;

						}
					}
				}
			}
		}
				 break;
		case MonsterManual: {
			Novice::DrawSprite(-255, 0, uiImage, 1, 1, 0, WHITE);

			// 怪物手册场景逻辑
			DrawMonsterManual();
			if (keys[DIK_Z] && preKeys[DIK_Z] == 0) {
				currentScene = Game; // 按下ESC返回游戏场景
			}
			// 绘制钥匙UI
			int baseX = kWindowWidth - 200;  // 右下角X起始坐标
			int baseY = kWindowHeight - 100; // 右下角Y起始坐标
			int iconSize = 50;               // 图标大小
			int spacing = 10;                // 图标间距

			// 绘制红钥匙
			Novice::DrawSprite(baseX, baseY, redKeyImage, 0.5f, 0.5f, 0.0f, WHITE);
			DrawNumber(localPlayer.redKeys, baseX + iconSize + spacing, baseY + iconSize / 4, 25, 25, WHITE);

			// 绘制蓝钥匙
			Novice::DrawSprite(baseX, baseY - iconSize - spacing, blueKeyImage, 0.5f, 0.5f, 0.0f, WHITE);
			DrawNumber(localPlayer.blueKeys, baseX + iconSize + spacing, baseY - iconSize - spacing + iconSize / 4, 25, 25, WHITE);

			// 绘制绿钥匙
			Novice::DrawSprite(baseX, baseY - 2 * (iconSize + spacing), greenKeyImage, 0.5f, 0.5f, 0.0f, WHITE);
			DrawNumber(localPlayer.greenKeys, baseX + iconSize + spacing, baseY - 2 * (iconSize + spacing) + iconSize / 4, 25, 25, WHITE);

			Novice::DrawSprite(kWindowWidth - 150, 80, floorImage, 1, 1, 0.0f, WHITE);
			DrawNumber(currentFloor + 1, kWindowWidth - 270, 87, 46, 46, WHITE);
			DrawNumber(localPlayer.health, kWindowWidth - 230, 227, 25, 25, WHITE); // 绘制生命值
			DrawNumber(localPlayer.attack, kWindowWidth - 230, 293, 25, 25, WHITE); // 绘制攻击力
			DrawNumber(localPlayer.defense, kWindowWidth - 230, 357, 25, 25, WHITE); // 绘制防御力
			DrawNumber(localPlayer.gold, kWindowWidth - 230, 422, 25, 25, WHITE); // 绘制金币数量
			Novice::DrawSprite(kWindowWidth - 270, 550, yan2Image, 1, 1, 0, WHITE);

			break;
		}
		case GameOver: {
			// 游戏结束场景逻辑
			InitializeGame(); // 重置游戏状态

			Novice::DrawSprite(0, 0, deadEndingImage, 1.0f, 1.0f, 0.0f, WHITE);

			if (keys[DIK_RETURN] && !preKeys[DIK_RETURN]) {
				currentScene = Title; // 按下回车键返回标题场景
				bgmTitlePlayHandle = -1;
			}
			break;
		}
		case Clear: {
			// 游戏通关场景逻辑
			InitializeGame(); // 重置游戏状态

			if (trueEnd) {
				Novice::DrawSprite(0, 0, trueEndingImage, 1.0f, 1.0f, 0.0f, WHITE);
			}
			else {
				Novice::DrawSprite(0, 0, normalEndingImage, 1.0f, 1.0f, 0.0f, WHITE);
			}
			// 按下 Enter 键返回标题界面
			if (preKeys[DIK_RETURN] == 0 && keys[DIK_RETURN] != 0 || preKeys[DIK_SPACE] == 0 && keys[DIK_SPACE] != 0) {
				bgmTitlePlayHandle = -1;

				currentScene = Title;
			}
			break;
		}
		case Menu: {
			// 菜单场景逻辑
			Novice::ScreenPrintf(100, 100, "Menu Scene");
			// 菜单的具体逻辑实现
			break;
		}
		case Battle: {



			break;

		}

		}




		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける

	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}



