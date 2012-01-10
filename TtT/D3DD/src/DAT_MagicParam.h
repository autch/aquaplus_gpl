
MAGIC_PARAM	MagicParam[MAGIC_MAX]={

	{"エレクトラム　", MAGIC_HOLY, 10,    30,  800, 256,  96, RANGE_SCOPE,  ATTR_SEI, 2.0f, 100,0, MG_TYPE_DAMAGE, 0, 9104, "神聖系攻撃魔法。\n魔に抗するという「琥珀金」を魔力で現出させる。"},
	{"エレクトリウム", MAGIC_HOLY, 11,   100, 1000, 256, 128, RANGE_SCOPE,  ATTR_SEI, 2.0f, 500,0, MG_TYPE_DAMAGE, 0, 9105, "神聖系攻撃魔法。\n天よりエレクトラムを召喚し、うちつける。"},
	{"グラヴィタス　", MAGIC_HOLY,  0,   150, 1000, 512, 160, RANGE_SCOPE,  ATTR_SEI, 3.0f,1200,0, MG_TYPE_DAMAGE, 0, 9103, "神聖系最強魔法。\n神の重力と呼ばれ、人で扱えるものはいないとされる。"},

	{"ニグレド　　　", MAGIC_HOLY, 20,    10,  400, 256,   0, RANGE_SELF,   ATTR_SEI, 1.0f,   0,0, MG_TYPE_DAMAGE, 0, 9106, "全身を変質させ、一切の攻撃を受け付けなくなるが、自らも移動しか出来なくなる。効果切れの後、弱体化する。"},
	{"アルペド　　　", MAGIC_HOLY, 21,    10,  400, 256,   0, RANGE_SELF,   ATTR_SEI, 1.0f,   0,0, MG_TYPE_DAMAGE, 0, 9107, "全身を変質させ、一切の攻撃を受け付けなくなるが、自らも行動が出来なくなる。効果切れの後、弱体化する。"},
	{"キリトニクス　", MAGIC_HOLY, 22,    10,  400, 256,   0, RANGE_SELF,   ATTR_SEI, 1.0f,   0,0, MG_TYPE_DAMAGE, 0, 9108, "全身を変質させ、物理攻撃を受け付けなくなるが、自らも物理攻撃が出来なくなる。効果切れの後、弱体化する。"},
	{"ルベド　　　　", MAGIC_HOLY, 23,    10,  400, 256,   0, RANGE_SELF,   ATTR_SEI, 1.0f,   0,0, MG_TYPE_DAMAGE, 0, 9109, "全身を変質させ、魔法の効果を受け付けなくなるが、自らも魔法が使えなくなる。効果切れの後、弱体化する。"},

	{"ピグマリオン　", MAGIC_HOLY, 40,    70,  400, 256,   0, RANGE_TAGET,  ATTR_SEI, 1.0f,   -1,0, MG_TYPE_HACK, 0, 9110, "目標を洗脳し、一時的に味方につける。\n失敗することもある。"},

	{"体力吸収　　　", MAGIC_DARK,  0,    20,  400, 256,   0, RANGE_TAGET,  ATTR_YAMI, 1.0f, 500,0, MG_TYPE_DAMAGE,    MG_TYPE_HP_GET, 9100, "目標にダメージを与え、同じだけ自分のHPに加える。"},
	{"魔力吸収　　　", MAGIC_DARK,  1,    40,  400, 256,   0, RANGE_TAGET,  ATTR_YAMI, 1.0f, 100,0, MG_TYPE_DAMAGE_MP, MG_TYPE_MP_GET, 9101, "目標にMPダメージを与え、同じだけ自分のMPに加える。"},
	{"呪い　　　　　", MAGIC_DARK, 10,    50,  800, 256,  64, RANGE_SCOPE,  ATTR_YAMI, 1.0f,   0,0, MG_TYPE_DAMAGE_MP, 0, 9102, "暗黒系攻撃魔法。MPにダメージを与える。" },

	{"爆発　　　　　", MAGIC_ZOKUSEI, 0,    10,  600, 256,     64, RANGE_SCOPE,  ATTR_KA, 2.0f,  10,0, MG_TYPE_DAMAGE, 0,9125, "炎属性魔法。\n火球が爆発し、辺りにいるものを炎に包む" },
	{"炎上　　　　　", MAGIC_ZOKUSEI, 1,    40,  800, 256+32,  96, RANGE_SCOPE,  ATTR_KA, 2.0f, 300,0, MG_TYPE_DAMAGE, 0,9126, "炎属性魔法。\n火柱が炎上し、その周りにいる者を焼き尽くす" },
	{"業火　　　　　", MAGIC_ZOKUSEI, 2,   100, 1000, 256+64, 128, RANGE_SCOPE,  ATTR_KA, 2.0f, 800,0, MG_TYPE_DAMAGE, 0,9127, "炎属性魔法。\n上空からの怒涛の熱エネルギーが降り注ぎ、辺り一帯を灰燼に帰す" },

	{"鉄球落とし　　", MAGIC_ZOKUSEI, 10,   10,  600, 256,     64, RANGE_SCOPE,  ATTR_TI, 2.0f,  10,0, MG_TYPE_DAMAGE, 0,9128, "地属性魔法。\nとげとげの痛い鉄球が降ってくる。" },
	{"粘菌　　　　　", MAGIC_ZOKUSEI, 11,   40,  800, 256+32,  96, RANGE_SCOPE,  ATTR_TI, 2.0f, 300,0, MG_TYPE_DAMAGE, 0,9129, "地属性魔法。\nどこからともなく現れた菌糸によるネバネバ攻撃。精神ダメージも尋常ではない" },
	{"奈落　　　　　", MAGIC_ZOKUSEI, 12,  100, 1000, 256+64, 128, RANGE_SCOPE,  ATTR_TI, 2.0f, 800,0, MG_TYPE_DAMAGE, 0,9130, "地属性魔法。\n奈落の門が開き、不幸な犠牲者をこの世ならざる場所へ連れて行くという" },

	{"氷結　　　　　", MAGIC_ZOKUSEI, 20,   10,  600, 256,     64, RANGE_SCOPE, ATTR_SUI, 2.0f,  10,0, MG_TYPE_DAMAGE, 0,9131, "水属性魔法。\n熱交換により冷気を発生させ、空間を氷結させる" },
	{"吹雪　　　　　", MAGIC_ZOKUSEI, 21,   40,  800, 256+32,  96, RANGE_SCOPE, ATTR_SUI, 2.0f, 300,0, MG_TYPE_DAMAGE, 0,9132, "水属性魔法。\n局地的に気象を暴走させて、猛吹雪を発生させる" },
	{"氷柱　　　　　", MAGIC_ZOKUSEI, 22,  100, 1000, 256+64, 128, RANGE_SCOPE, ATTR_SUI, 2.0f, 800,0, MG_TYPE_DAMAGE, 0,9133, "水属性魔法。\n氷山から氷柱を転移させると共に空間の動きを止め、一帯を氷漬けにする" },

	{"カマイタチ　　", MAGIC_ZOKUSEI, 30,   10,  600, 256,     64, RANGE_SCOPE, ATTR_HUU, 2.0f,  10,0, MG_TYPE_DAMAGE, 0,9134, "風属性魔法。\n疾風を巻き起こし、剣で裂くような傷を負わせる" },
	{"落雷　　　　　", MAGIC_ZOKUSEI, 31,   40,  800, 256+32,  96, RANGE_SCOPE, ATTR_HUU, 2.0f, 300,0, MG_TYPE_DAMAGE, 0,9135, "風属性魔法。\n雷を落とす。直撃を喰らった者は消し炭と化すであろう" },
	{"竜巻　　　　　", MAGIC_ZOKUSEI, 32,  100, 1000, 256+64, 128, RANGE_SCOPE, ATTR_HUU, 2.0f, 800,0, MG_TYPE_DAMAGE, 0,9136, "風属性魔法。\n全ての物を巻き込む竜巻。この巻き添えになった者は挽肉と化す" },

	{"臨界　　　　　", MAGIC_ZOKUSEI, 40,  200, 1200, 384, 160, RANGE_SCOPE, ATTR_NOT, 3.0f, 1500,0, MG_TYPE_DAMAGE, 0,9137, "無属性攻撃魔法。\nあらゆる物質から最大のエネルギーを引き出す。ドルイドにのみ伝えられる禁呪。" },

	{"恵みの雫　　　", MAGIC_ZOKUSEI, 50,  100,  800, 256, 128, RANGE_SCOPE, ATTR_SUI, 2.0f, 150,0, MG_TYPE_HEAL,   0,9138, "水属性回復魔法。\n広範囲を回復するが、速度が遅くMP効率も悪い。" },
	{"転移　　　　　", MAGIC_ZOKUSEI, 51,   50,  300, 384,   0, RANGE_TAGET, ATTR_NOT, 1.0f,   0,0, MG_TYPE_TENI,   0,9139, "風属性移動魔法。\nターゲットと位置を入れ替える。" },

	{"攻撃速度アップ", MAGIC_ZOKUSEI, 60,   40, 600, 256,   0,  RANGE_TAGET, ATTR_NOT, 1.0f, 50,0, MG_TYPE_WSP,     0,9140, "支援強化魔法。\n武器攻撃間隔（WSP）を下げ、高速化する。" },
	{"移動速度アップ", MAGIC_ZOKUSEI, 61,   40, 600, 256,   0,  RANGE_TAGET, ATTR_NOT, 1.0f, 50,0, MG_TYPE_MOV,     0,9141, "支援強化魔法。\n移動速度（MOV）を上げる。"},

	{"物理攻撃アップ", MAGIC_ZOKUSEI, 62,   30, 400, 256,   0,  RANGE_TAGET, ATTR_NOT, 1.0f, 20,0, MG_TYPE_ATK,     0,9142, "支援強化魔法。\n物理攻撃力（ATK）を２０％上げる。" },
	{"魔法攻撃アップ", MAGIC_ZOKUSEI, 63,   30, 400, 256,   0,  RANGE_TAGET, ATTR_NOT, 1.0f, 20,0, MG_TYPE_MGK,     0,9143, "支援強化魔法。\n魔法攻撃力（MGK）を２０％上げる。" },

	{"なにげない回復　", MAGIC_SENSEI,  0,   8,  400, 256,  0,  RANGE_TAGET,  ATTR_NOT, 1.0f,   100,0, MG_TYPE_HEAL,   0,9115, "回復魔法。\nなにげなく回復する。\n最初はこれで十分！"  },
	{"いい感じに回復　", MAGIC_SENSEI,  1,  20,  500, 256,  0,  RANGE_TAGET,  ATTR_NOT, 1.0f,   200,0, MG_TYPE_HEAL,   0,9116, "回復魔法。\nいい感じに回復する。\n終盤でも重宝する回復の要。" },
	{"ありえない回復　", MAGIC_SENSEI,  2,  70,  600, 256,  0,  RANGE_TAGET,  ATTR_NOT, 1.0f,  9999,0, MG_TYPE_HEAL,   0,9117, "回復魔法。\nありえないぐらい回復する。\nどんな傷でも一撃昇天！\n昇天？" },
	
	{"みんな回復　　　", MAGIC_SENSEI, 10, 120,  600, 128, 64, RANGE_SCOPE,   ATTR_NOT, 2.0f,   200,0, MG_TYPE_HEAL,   0,9117, "回復魔法。\n範囲内を、いい感じに回復する。MP消費多すぎ。" },
	
	{"星屑　　　　　　", MAGIC_SENSEI, 20, 150, 1200, 384,  96, RANGE_SCOPE,  ATTR_HOSI,2.0f,  1000,0, MG_TYPE_DAMAGE, 0,9118, "星属性攻撃魔法。\n神秘の宇宙エネルギー。" },
	{"流星　　　　　　", MAGIC_SENSEI, 21, 300, 1500, 512, 128, RANGE_SCOPE,  ATTR_HOSI,2.0f,  1500,0, MG_TYPE_DAMAGE, 0,9119, "星属性攻撃魔法。\n天空から「お星様」を召喚し、落下させる。" },
	
	{"全魔法解除　　　", MAGIC_SENSEI, 30,  10, 400, 256,   0,  RANGE_TAGET,  ATTR_NOT, 1.0f,     0,0, MG_TYPE_DISPEL, 0,9120, "支援魔法。\n強化効果も、弱体効果も、全て解除する。" },

	{"攻撃速度ダウン　", MAGIC_SENSEI, 40,  40, 400, 256,   0,  RANGE_TAGET,  ATTR_NOT, 1.0f,   -50,0, MG_TYPE_WSP,    0,9121, "支援弱体魔法。\n武器攻撃間隔（WSP）を上げ、速度を落とす。" },
	{"移動速度ダウン　", MAGIC_SENSEI, 41,  40, 400, 512,   0,  RANGE_TAGET,  ATTR_NOT, 1.0f,   -50,0, MG_TYPE_MOV,    0,9122, "支援弱体魔法。\n移動速度（MOV）を下げる。" },
	
	{"物理シールド　　", MAGIC_SENSEI, 42,  30, 600, 256,   0,  RANGE_TAGET,  ATTR_NOT, 1.0f,    30,0, MG_TYPE_DEF,    0,9123, "支援強化魔法。\n物理防御力（DEF）を３０％上げる。" },
	{"魔法シールド　　", MAGIC_SENSEI, 43,  30, 600, 256,   0,  RANGE_TAGET,  ATTR_NOT, 1.0f,    50,0, MG_TYPE_RGS,    0,9124, "支援強化魔法。\n魔法防御力（RGS）を５０％上げる。" },

	{"ドラゴンブレス　", MAGIC_SKILL,    0,  50, 400,   0, 384,  RANGE_FAN,   ATTR_KA,  2.0f,   500,  0, MG_TYPE_DAMAGE, 0, 9111, "俺のビッグマグナムが火を噴くぜ！"},
	{"アクアリウム　　", MAGIC_MONSTAR,  1,  20, 500, 192, 128,  RANGE_SCOPE, ATTR_SUI, 1.0f,   100,-20, MG_TYPE_DAMAGE, MG_TYPE_RGS, 9112, "水系妖精魔法。\n水の泡で包み込む。ダメージと共に魔法防御力を低下させる。"},
	{"ポイズン　　　　", MAGIC_MONSTAR,  2,  20, 500, 192, 128,  RANGE_SCOPE, ATTR_TI,  1.0f,   100,-20, MG_TYPE_DAMAGE, MG_TYPE_DEF, 9113, "土系魔法。\n毒の泉を発生させ、ダメージと共に防御力を低下させる。"},
	{"目くらまし　　　", MAGIC_MONSTAR,  3,  40, 600, 192, 128,  RANGE_SCOPE, ATTR_NOT, 1.0f,     0,  0, MG_TYPE_DAMAGE, MG_TYPE_STAN, 9114, "強烈な光と音で、一定時間行動不能にする。"},


	{"レーザー小　　　", MAGIC_SKILL, 10,  50, 500, 192,   64,  RANGE_SCOPE, ATTR_NOT, 1.0f, -200,0, MG_TYPE_DAMAGE, 0,8129, "ビビー" },
	{"レーザー中　　　", MAGIC_SKILL, 11,  10, 400, 256,    0,  RANGE_FAN,   ATTR_NOT, 1.0f, 0,0, MG_TYPE_DAMAGE, 0,8129, "ビビビー" },
	{"レーザー大　　　", MAGIC_SKILL, 12,   0, 400,   0,  512,  RANGE_FAN,   ATTR_SEI, 1.0f, 1,0, MG_TYPE_DAMAGE, 0,8129, "ビビビビー" },

	{"ラスボスマッシュ", MAGIC_SKILL, 13,    0, 400, 64,   64,  RANGE_SCOPE, ATTR_NOT, 1.0f, 1,0, MG_TYPE_DAMAGE, 0,-1, "オラッ" },


	{"ラスボス爆発　　", MAGIC_MONSTAR, 20,  10, 400, 256,   0,  RANGE_SCOPE, ATTR_NOT, 2.0f, 0,0, MG_TYPE_DAMAGE, 0,-1, "ちゅどーん" },


	{"戦士のバラッド　　　", MAGIC_BARD, 0,  1, 0,  0, 256,  RANGE_SELF_SCOPE, ATTR_NOT, 1.0f, 20, -10, MG_TYPE_ATK, MG_TYPE_MGK,-1, "叙事詩。\n物理攻撃力を上げ、魔法攻撃力を低下させる。" },
	{"臆病者のフーガ　　　", MAGIC_BARD, 1,  1, 0,  0, 256,  RANGE_SELF_SCOPE, ATTR_NOT, 1.0f, 20, -10, MG_TYPE_DEF, MG_TYPE_RGS,-1, "遁走曲。\n物理防御力を上げ、魔法防御力を低下させる。" },

	{"風のラプソディ　　　", MAGIC_BARD, 2,  1, 0,  0, 256,  RANGE_SELF_SCOPE, ATTR_NOT, 1.0f, 50,  0, MG_YUMI_DEF, 0,-1, "狂詩曲。\n強風を巻き起こし弓攻撃を大幅に減衰する。" },

	{"盗賊のマーチ　　　　", MAGIC_BARD, 3,  1, 0,  0, 512,  RANGE_SELF_SCOPE, ATTR_NOT, 1.0f, 10, -10, MG_TYPE_MOV, MG_TYPE_WSP,-1, "行進曲。\n移動速度を上げ、攻撃速度を低下させる。" },
	{"英雄達のシンフォニー", MAGIC_BARD, 4,  1, 0,  0, 512,  RANGE_SELF_SCOPE, ATTR_NOT, 1.0f, 10, -10, MG_TYPE_WSP, MG_TYPE_MOV,-1, "交響曲。\n攻撃速度を上げ、移動速度を低下させる。" },

	{"春のソナタ　　　　　", MAGIC_BARD, 5,  2, 0,  0, 256,  RANGE_SELF_SCOPE, ATTR_NOT, 1.0f, 100, 0, MG_TYPE_HEAL,   0,-1, "奏鳴曲。\n徐々にHPを回復する。" },
	{"星屑のノクターン　　", MAGIC_BARD, 6,  2, 0,  0, 256,  RANGE_SELF_SCOPE, ATTR_NOT, 1.0f,   5, 0, MG_TYPE_MP,     0,-1, "夜想曲。\n徐々にMPを回復する。" },

	{"妖精達のトッカータ　", MAGIC_BARD, 7,  1, 0,  0, 256,  RANGE_SELF_SCOPE, ATTR_NOT, 1.0f, 20, -10, MG_TYPE_MGK, MG_TYPE_ATK,-1, "即興曲。\n魔法攻撃力を上げ、物理攻撃力を低下させる。" },
	{"恋人達のセレナーデ　", MAGIC_BARD, 8,  1, 0,  0, 256,  RANGE_SELF_SCOPE, ATTR_NOT, 1.0f, 20, -10, MG_TYPE_RGS, MG_TYPE_DEF,-1, "小夜曲。\n魔法防御力を上げ、物理防御力を低下させる。" },

	{"堕天使のレクイエム　", MAGIC_BARD, 9,  2, 0,  0, 256,  RANGE_SELF_SCOPE, ATTR_NOT, 1.0f, 100, 0, MG_TYPE_DAMAGE, 0,-1, "鎮魂曲。\n徐々にダメージを与える。" },
	
	{"アロウンインザダーク", MAGIC_BARD,10,  4, 0,  0, 256,  RANGE_SELF_SCOPE, ATTR_NOT, 1.0f, 40, 40, MG_TYPE_ATK, MG_TYPE_DEF,-1, "伝説の歌。\n攻撃力と防御力を共に強化。\n「何か」に似てるのは気にしない。" },

	
	{"悪口　　　　", MAGIC_SKILL, 100,  10, 100, 256,   0,  RANGE_TAGET,      ATTR_NOT, 1.0f,  0,0, SK_TOUHATU, 0,-1, "挑発行為で相手の注意をひきつけます。" },
	{"気合　　　　", MAGIC_SKILL, 101,  20, 100,   0,   0,  RANGE_SELF,       ATTR_NOT, 1.0f, 20,0, MG_TYPE_DEF, 0,-1, "防御力が上がります。" },
	{"罵詈雑言　　", MAGIC_SKILL, 102,  30, 100,   0, 256,  RANGE_SELF_SCOPE, ATTR_NOT, 1.0f,  0,0, SK_TOUHATU,   0,-1, "周囲の注意をひきつけます。下手に使えば自殺行為です。" },
	{"怒号　　　　", MAGIC_SKILL, 103,  50, 100,   0,   0,  RANGE_ALL,        ATTR_NOT, 1.0f,  0,0, SK_ALL_ATACK, 0,-1, "マップ上の待機型の敵ユニットを、全て突撃状態にします。\n楽な敵とは即決勝負！" },
	
	{"予測　　　　", MAGIC_SKILL, 200,  10, 100,   0,   0,  RANGE_SELF,   ATTR_NOT, 1.0f,   1,0, SK_DEF_HP_0, 0,-1, "次、物理攻撃を受けた時、一回だけ、ダメージを０にします。魔法攻撃を受けると消滅します。" },
	{"魔法予測　　", MAGIC_SKILL, 201,  10, 100,   0,   0,  RANGE_SELF,   ATTR_NOT, 1.0f,   1,0, SK_DEF_MP_0, 0,-1, "次、魔法攻撃を受けた時、一回だけ、ダメージを０にします。物理攻撃を受けると消滅します。" },
	{"見切り　　　", MAGIC_SKILL, 202,  30, 100,   0,   0,  RANGE_SELF,   ATTR_NOT, 1.0f, 100,0, MG_TYPE_AVD, 0,-1, "自分の回避能力を高めます。" },
	{"後の先　　　", MAGIC_SKILL, 203,  50, 100,   0,   0,  RANGE_SELF,   ATTR_NOT, 1.0f,   1,0, SK_DEF_HANGEKI, 0,-1, "待機状態時、敵の攻撃に対して、WSPに関係なく反撃を行う。行動中は無効。" },

	
	{"集中　　　　", MAGIC_SKILL, 300,  10, 100,   0,   0,  RANGE_SELF,   ATTR_NOT, 1.0f, 100,0, MG_TYPE_HIT, 0,-1, "一定時間、攻撃が命中しやすくなります。" },
	{"全速　　　　", MAGIC_SKILL, 301,  30, 100,   0,   0,  RANGE_SELF,   ATTR_NOT, 1.0f, 100,0, MG_TYPE_MOV, 0,-1, "一定時間、移動力を倍にします。" },
	{"速射　　　　", MAGIC_SKILL, 302,  50, 100,   0,   0,  RANGE_SELF,   ATTR_NOT, 1.0f,  50,0, MG_TYPE_WSP, 0,-1, "武器攻撃間隔を短くします。" },
	
	{"自然の教え　", MAGIC_SKILL, 400,  10, 100,   0,   0,  RANGE_SELF,   ATTR_NOT, 1.0f,   1,0, SK_ITEM_BAI,  0,-1, "消費アイテムの効果が倍になる。" },
	{"サバイバル　", MAGIC_SKILL, 401,  20, 100,   0,   0,  RANGE_SELF,   ATTR_NOT, 1.0f, 100,0, MG_TYPE_HEAL, 0,-1, "自分のHPを回復します。" },
	{"狙撃　　　　", MAGIC_SKILL, 402,  30, 100,   0,   0,  RANGE_SELF,   ATTR_NOT, 1.0f,  50,0, MG_TYPE_ATK,  0,-1, "攻撃力をアップします。" },
	
	

	
	{"剣技：無双烈破　　　", MAGIC_SKILL, 900,  50, 400, 96,  0,  RANGE_TAGET,       ATTR_NOT, 1.0f, 300,0, MG_TYPE_DAMAGE, 0,-1, "目標を完膚なきまでに叩き切ります。" },
	
	{"槍術：一閃　　　　　", MAGIC_SKILL, 901,  50, 400, 0,  128,  RANGE_SELF_SCOPE, ATTR_NOT, 1.0f, 400,0, MG_TYPE_DAMAGE, 0,-1, "周囲の敵を、なぎ払います。" },
	
	{"鎚技：どっかーん　　", MAGIC_SKILL, 902,  50, 400, 80,  64,  RANGE_SCOPE,      ATTR_NOT, 1.0f, -200,0, MG_TYPE_DAMAGE, 0,-1, "どっかん。どっかん。どっかーん。" },
	
	{"弓術：高速乱射　　　", MAGIC_SKILL, 903,  50, 400, 256,  64,  RANGE_SCOPE,     ATTR_NOT, 1.0f, -200,0, MG_TYPE_DAMAGE, 0,-1, "目標周辺の敵にテキトーに撃ちまくり。" },
};

