#include "Achi.h"
#include "ui_Achi.h"

#include <QMessageBox>
#include <QSignalMapper>

Achi::Achi (QWidget *parent) : QMainWindow(parent), ui(new Ui::Achi) {
	ui->setupUi(this);

	QObject::connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(reset()));
	QObject::connect(ui->actionQuit, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
	QObject::connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(showAbout()));

	// Load the pieces.
	QSignalMapper *map = new QSignalMapper(this);
	for (int id = 0; id < 9; id++) {
		int r = id / 3;
		int c = id % 3;

		Hole *hole = this->findChild<Hole *>(QString("hole%1%2").arg(r).arg(c));
		Q_ASSERT(hole != nullptr);

		m_holes[id] = hole;
		map->setMapping(hole, id);
		QObject::connect(hole, SIGNAL(clicked()), map, SLOT(map()));
	}

	QObject::connect(map, SIGNAL(mapped(int)), this, SLOT(play(int)));

	resetGameState();

	// Compact the layout of the widgets.
	this->adjustSize();

	// Set a fixed window size.
	this->setFixedSize(this->size());
}

Achi::~Achi () {
	delete ui;
}

void Achi::updateState () {
	if (m_holes[0]->isFilled() && (
		(m_holes[0]->state() == m_holes[1]->state() && m_holes[0]->state() == m_holes[2]->state()) ||
		(m_holes[0]->state() == m_holes[3]->state() && m_holes[0]->state() == m_holes[6]->state()) ||
		(m_holes[0]->state() == m_holes[4]->state() && m_holes[0]->state() == m_holes[8]->state()))
	) {
		m_gameEnded = true;
		setWinner(m_holes[0]);
		return;
	}

	if (m_holes[2]->isFilled() && m_holes[2]->state() == m_holes[4]->state() && m_holes[2]->state() == m_holes[6]->state()) {
		m_gameEnded = true;
		setWinner(m_holes[2]);
		return;
	}

	for (int i = 0; i < 2; i++) {
		if (m_holes[1 + i]->isFilled() && m_holes[1 + i]->state() == m_holes[4 + i]->state() && m_holes[1 + i]->state() == m_holes[7 + i]->state()) {
			m_gameEnded = true;
			setWinner(m_holes[1 + i]);
			return;
		}

		if (m_holes[3 + (3 * i)]->isFilled() && m_holes[3 + (3 * i)]->state() == m_holes[4 + (3 * i)]->state() && m_holes[3 + (3 * i)]->state() == m_holes[5 + (3 * i)]->state()) {
			m_gameEnded = true;
			setWinner(m_holes[3 + (3 * i)]);
			return;
		}
	}

	QString fase = m_canMove ? "Fase de mover" : "Fase de colocar";
	QString player = m_isPlayer1 ? "vermelho" : "azul";
	ui->statusbar->showMessage(fase + ": vez do jogador " + player + ".");
}

void Achi::play (int index) {
	if (m_gameEnded)
		return;

	Hole *hole = m_holes[index];
	Q_ASSERT(hole != nullptr);

	if (!m_canMove) {
		if (!hole->isEmpty())
			return;

		hole->setState(m_isPlayer1 ? Hole::RedState : Hole::BlueState);

		m_placedPieces++;
		m_isPlayer1 = !m_isPlayer1;

		if (m_placedPieces == 6)
			m_canMove = true;
	} else {
		if (m_selectedPiece < 0) {
			if (m_holes[index]->isEmpty() || m_holes[index]->isPlayer1() != m_isPlayer1)
				return;

			// Highlight possible moves
			for (int id = 0; id < 9; id++) {
				if (m_holes[id]->isEmpty() && m_holes[id]->isBeside(m_holes[index]->row(), m_holes[index]->col())) {
					m_holes[id]->setState(Hole::SelectableState);

					// Only select the piece if there is at least one valid move
					m_selectedPiece = index;
				}
			}
		} else if (m_holes[m_selectedPiece]->state() == m_holes[index]->state()) {
			// Unselect current piece
			for (int id = 0; id < 9; id++) {
				if (m_holes[id]->isSelectable())
					m_holes[id]->reset();
			}

			const bool diferentPiece = m_selectedPiece != index;
			m_selectedPiece = -1;

			// If the user selected another piece
			if (diferentPiece)
				play(index);
		} else if (m_holes[index]->isSelectable()) {
			m_holes[m_selectedPiece]->reset();
			m_holes[index]->setState(m_isPlayer1 ? Hole::RedState : Hole::BlueState);
			for (int id = 0; id < 9; id++) {
				if (m_holes[id]->isSelectable())
					m_holes[id]->reset();
			}

			m_selectedPiece = -1;
			m_isPlayer1 = !m_isPlayer1;
		}
	}

	updateState();
}

void Achi::setWinner (Hole *hole) {
	QString player = hole->isPlayer1() ? "vermelho" : "azul";
	ui->statusbar->showMessage("Fim de jogo: o jogador " + player + " venceu!");
	QMessageBox::information(this, tr("Vencedor"), tr("Parabéns jogador %1, você venceu!").arg(player));
}

void Achi::reset () {
	for (int id = 0; id < 9; id++) {
		Hole *hole = m_holes[id];
		Q_ASSERT(hole != nullptr);

		m_holes[id]->reset();
	}

	resetGameState();
}

void Achi::resetGameState () {
	// Reset game state variables
	m_canMove = false;
	m_isPlayer1 = true;
	m_gameEnded = false;
	m_placedPieces = 0;
	m_selectedPiece = -1;
	updateState();
}

void Achi::showAbout () {
	QMessageBox::information(this, tr("Sobre"), tr("Achi\n\nLucas Rassilan Vilanova - lucasrsv1@gmail.com.br\n\nLinguagens de Programação - TP2\nCEFET-MG ©2021"));
}
