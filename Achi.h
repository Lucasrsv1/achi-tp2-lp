#ifndef ACHI_H
#define ACHI_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
	class Achi;
}
QT_END_NAMESPACE

class Hole;

class Achi : public QMainWindow {
	Q_OBJECT

	public:
		Achi(QWidget *parent = nullptr);
		virtual ~Achi();

	private:
		Ui::Achi *ui;
		Hole *m_holes[9];

		bool m_gameEnded;
		bool m_canMove;
		bool m_isPlayer1;

		int m_placedPieces;
		int m_selectedPiece;

		void updateState();
		void resetGameState();
		void setWinner(Hole *hole);

	private slots:
		void play(int id);
		void reset();
		void showAbout();
};

#endif // ACHI_H
