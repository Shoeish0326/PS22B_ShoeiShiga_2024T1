# include <Siv3D.hpp>
# include <time.h>

/*
	よりC++ライクな書き方
	・クラスベース
	・継承を行う
*/

//==============================
// 前方宣言
//==============================
class GameManager;
class Ball;
class Bricks;
class Paddle;



//==============================
// 定数
//==============================
namespace constants
{
	
	namespace brick
	{
		/// @brief ブロックのサイズ
		constexpr Size SIZE{ 40, 20 };

		/// @brief ブロックの数　縦
		constexpr int Y_COUNT = 5;

		/// @brief ブロックの数　横
		constexpr int X_COUNT = 20;

		/// @brief 合計ブロック数
		constexpr int MAX = Y_COUNT * X_COUNT;
	}

	namespace ball
	{
		/// @brief ボールの速さ
		constexpr double SPEED = 480.0;
		const int ballStock = 3;
		const Vec2 ballStartPos = { 400,400 };
	}

	namespace paddle
	{
		/// @brief パドルのサイズ
		constexpr Size SIZE{ 60, 10 };
		constexpr int defaultYPos = 500;
		/*enum paddleState
		{
			Default,
			pullingDown,
			pullDown,
			PushUp,
			CoolDown
		};*/
	}

	namespace reflect
	{
		/// @brief 縦方向ベクトル
		constexpr Vec2 VERTICAL{ 1, -1 };
		/// @brief 横方向ベクトル
		constexpr Vec2 HORIZONTAL{ -1,  1 };
	}
}

//==============================
// クラス宣言
//==============================

/// @brief ボール
class Ball final {
private:
	/// @brief 速度
	Vec2 velocity;

	/// @brief ボール
	//Circle* pBall;
	Circle ball;
	//Circle ball[] = new Circle[3];

public:
	/// @brief コンストラクタ
	Ball() : velocity({ 0, -constants::ball::SPEED }), ball({ constants::ball::ballStartPos, 8 }) {}

	/// @brief デストラクタ
	~Ball() {}

	/// @brief 更新
	void Update()
	{
		ball.moveBy(velocity * Scene::DeltaTime());		
	}

	/// @brief 描画
	void Draw() const
	{
		ball.draw();
	}

	Circle GetCircle() const
	{
		return ball;
	}

	Vec2 GetVelocity() const
	{
		return velocity;
	}

	/// @brief 新しい移動速度を設定
	/// @param newVelocity 新しい移動速度
	void SetVelocity(Vec2 newVelocity)
	{
		using namespace constants::ball;
		velocity = newVelocity.setLength(SPEED);
	}

	/// @brief 反射
	/// @param reflectVec 反射ベクトル方向 
	void Reflect(const Vec2 reflectVec)
	{
		velocity *= reflectVec;
	}

	Vec2 GetVelocity()
	{
		return velocity;
	}

	void Reset() {
		ball.setPos(constants::ball::ballStartPos);
		velocity.x = 0;
		velocity.y = -constants::ball::SPEED;
	}
};

/// @brief ブロック
class Bricks final {
private:
	/// @brief ブロックリスト
	Rect brickTable[constants::brick::MAX];
	int b_life[constants::brick::MAX];
	int currentBlockNum = 0;
	int score = 0;

public:
	/// @brief コンストラクタ
	Bricks() : currentBlockNum{ constants::brick::MAX }
	{
		using namespace constants::brick;
		using namespace std;
		
		for (int y = 0; y < Y_COUNT; ++y) {
			for (int x = 0; x < X_COUNT; ++x) {

				int index = y * X_COUNT + x;
				brickTable[index] = Rect{x * SIZE.x,60 + y * SIZE.y,SIZE};
				int num = Random(100);
				if (num > 80)
				{
					b_life[index] = 2;
				}
				else
				{
					b_life[index] = 1;
				}
			}
		}
	}

	/// @brief デストラクタ
	~Bricks()
	{
		currentBlockNum--;
	}
	/*void SetLife(int ballPowerLv)
	{
		b_life[] = value;
	}*/
	/// @brief 衝突検知
	void Intersects(Ball* const target);

	/// @brief 描画
	void Draw() const
	{
		using namespace constants::brick;

		for (int i = 0; i < MAX; ++i)
		{
			brickTable[i].stretched(-1).draw(HSV{b_life[i] * 50 + 10});
			//brickTable[i].y - 40
		}
	}
		
	void GetBlockNum(int& num)
	{
		 num = currentBlockNum;
	}

	int GetBlockNum()
	{
		return currentBlockNum;
	}
	int GetScore()
	{
		return score;
	}
};

class GameManager final
{
private:
	bool gameOver = false;
	int ballStock;
	int scoreSum; 
	Vec2 windowSize;
	
public:
	const Font font{ FontMethod::MSDF, 48 };

	GameManager() : ballStock(3),scoreSum(0), windowSize({ 800,600 })
	{
		Window::Resize(windowSize.x, windowSize.y);		
	}

	void GameContinue(Ball* ball, Bricks* bricks)
	{		
		font( U"BallStock: {}"_fmt(ballStock)).draw(15,10,10);
		font(U"Score: {}"_fmt(bricks->GetScore())).draw(15,10,25);
		if (ball) {
			if (ball->GetCircle().y - 50 >= constants::paddle::defaultYPos)
			{
				if (ballStock > 0)
				{
					ball->Reset();
					//delete ball;
					//ball = new Ball();
					ballStock--;
				}
				else
				{
					gameOver = true;					
				}						
			}
		}
		
		if (bricks -> GetBlockNum() < 0)
		{
			gameOver = true;			
		}
	}
	void GameOver(Ball* ball, Bricks* bricks)
	{
		if (gameOver)
		{			
			scoreSum = bricks -> GetScore();
			delete ball, bricks;			
		}
	}

	void SetBallNum(int num)
	{
		ballStock = num;
	}

	bool GetGameOverFlag()
	{
		return gameOver;
	}

	int GetResult()
	{
		return scoreSum;
	}

	Vec2 GetWindowSize()
	{
		return windowSize;
	}
};

/// @brief パドル
class Paddle final
{
private:
	Rect paddle;
	double dispY = 0; 
	const double dispYMax = 20; 
	//double paddlePushRange = 150;
	bool isFixed = false;	
	
public:
	/// @brief コンストラクタ
	Paddle() : paddle(Rect(Arg::center(Cursor::Pos().x,constants::paddle::defaultYPos ), constants::paddle::SIZE)) {}

	/// @brief デストラクタ
	~Paddle() {}

	/// @brief 衝突検知
	void Intersects(Ball* const target) const;

	/// @brief 更新
	void Update()
	{
		ClearPrint();
		paddle.x = Cursor::Pos().x - (constants::paddle::SIZE.x / 2);
		MouseButtonInput();		
	}

	void MouseButtonInput()
	{
		if (MouseL.pressed())
		{			
			if (dispY < dispYMax)
			{
				paddle.y += 3;
				dispY += 3;
			}
			else
			{
				if (!isFixed)
				{
					paddle.y -= dispY;
					paddle.y += dispYMax;
					dispY = dispYMax;
					isFixed = true;
				}				
			}
			
		}
	
		if (MouseL.up())
		{
			/*for (int i = 0; i < 3; i++)
			{
				paddle.y -= disY;
			}
			for (int i = 0; i < 50; i++)
			{
				paddle.y += disY/25;
			}*/
			paddle.y -= dispY;
			dispY = 0;
			isFixed = false;
			//paddle.y -= 2 * disY;

			//// 変位0に戻すためにアニメーションする
			//const double targetY = paddle.y + 2 * disY;
			//const double step = disY / 10; // 変位を戻すためのステップ
			//disY = 0;
			//isFixed = false;

			//// タイマーやフレームごとの更新を使ってパドルを元の位置に戻す
			//while (paddle.y < targetY)
			//{
			//	paddle.y += step;
			//	System::Update();
			//	paddle.draw();
			//}
			//paddle.y = targetY;
		}
	}

	/// @brief 描画
	void Draw() const
	{
		paddle.rounded(3).draw();
	}
};

/// @brief 壁
class Wall
{
public:
	/// @brief 衝突検知
	static void Intersects(Ball* target)
	{
		using namespace constants;

		if (!target)
		{
			return;
		}

		auto velocity = target->GetVelocity();
		auto ball = target->GetCircle();

		// 天井との衝突を検知
		if ((ball.y < 0) && (velocity.y < 0))
		{
			target->Reflect(reflect::VERTICAL);
		}

		// 壁との衝突を検知
		if (((ball.x < 0) && (velocity.x < 0))
			|| ((Scene::Width() < ball.x) && (0 < velocity.x)))
		{
			target->Reflect(reflect::HORIZONTAL);
		}
	}
};

//==============================
// 定義
//==============================
void Bricks::Intersects(Ball* const target) {
	using namespace constants;
	using namespace constants::brick;

	if (!target) {
		return;
	}

	auto ball = target->GetCircle();

	for (int i = 0; i < MAX; ++i) {
		// 参照で保持
		Rect& refBrick = brickTable[i];
		int& bLife = b_life[i];
		// 衝突を検知
		if (refBrick.intersects(ball))
		{
			bLife -= 1;
			score++;
			if (target->GetVelocity().length() > constants::ball::SPEED)//ボールの速さが初期値以上の場合
			{
				bLife -= 1;
				score++;
			}
			// ブロックの上辺、または底辺と交差
			if (refBrick.bottom().intersects(ball)
				|| refBrick.top().intersects(ball))
			{
				target->Reflect(reflect::VERTICAL);
			}
			else // ブロックの左辺または右辺と交差
			{
				target->Reflect(reflect::HORIZONTAL);
			}

			if (bLife <= 0)
			{
				refBrick.y -= 600;
			}
			// あたったブロックは画面外に出す
			

			// 同一フレームでは複数のブロック衝突を検知しない
			break;
		}
	}
}

void Paddle::Intersects(Ball* const target) const
{
	if (!target)
	{
		return;
	}

	auto velocity = target->GetVelocity();
	auto ball = target->GetCircle();

	if ((0 < velocity.y) && paddle.intersects(ball))
	{
		target->SetVelocity(Vec2{(ball.x - paddle.center().x) * 10,-velocity.y});
	}
}

//==============================
// エントリー
//==============================
void Main()
{
	srand(time(NULL));

	GameManager gm;	
	Paddle paddle;

	Bricks* pbricks = NULL; //ポインタ変数はNULLで初期化
	Ball* pball = NULL;

	pbricks = new Bricks();
	pball = new Ball();	

	while (System::Update())
	{		
		if (!gm.GetGameOverFlag()) //ゲーム終了のフラグがfalseの場合の処理
		{
			//==============================
			// 更新
			//==============================
			paddle.Update();
			pball->Update();
			gm.GameContinue(pball, pbricks);

			//==============================
			// コリジョン
			//==============================
			pbricks->Intersects(pball);
			Wall::Intersects(pball);
			paddle.Intersects(pball);
			//DeadZone::Intersects(&ball);

			//==============================
			// 描画
			//==============================
			pbricks->Draw();
			pball->Draw();
			paddle.Draw();
			gm.GameOver(pball, pbricks);
		}
		else //gamemanagerがゲーム終了のフラグをtrueにしたあとの処理
		{
			gm.font(U"Your score is {}!"_fmt(gm.GetResult())).drawAt(50, { gm.GetWindowSize().x / 2,80 });
			gm.font(U"Press Enter").drawAt(30, { gm.GetWindowSize().x / 2,gm.GetWindowSize().y / 2 + 30 });
			if (KeyEnter.down())
			{
				System::Exit();
			}
		}
		
	}
}
