#include "Hole.h"

Hole::Hole (QWidget *parent) : QPushButton(parent), m_row(0), m_col(0), m_state(Hole::EmptyState) {
	this->updateHole(m_state);
	QObject::connect(this, SIGNAL(stateChanged(State)), this, SLOT(updateHole(State)));
}

Hole::~Hole () { }

void Hole::setState (State state) {
	if (m_state != state) {
		m_state = state;
		emit stateChanged(state);
	}
}

bool Hole::isEmpty () {
	return m_state == EmptyState;
}

bool Hole::isSelectable () {
	return m_state == SelectableState;
}

bool Hole::isFilled () {
	return m_state == RedState || m_state == BlueState;
}

bool Hole::isPlayer1 () {
	return m_state == RedState;
}

bool Hole::isBeside (int row, int col) {
	if (this->row() == 1 && this->col() == 1)
		return abs(this->row() - row) <= 1 && abs(this->col() - col) <= 1;

	const bool validSameRowOrCol = abs(this->row() - row) + abs(this->col() - col) <= 1;
	const bool diagonal = row == 1 && col == 1 && this->row() != 1 && this->col() != 1;

	return validSameRowOrCol || diagonal;
}

void Hole::reset () {
	m_state = Hole::EmptyState;
	this->updateHole(m_state);
}

QPixmap Hole::stateToPixmap (State state) {
	switch (state) {
		case Hole::EmptyState:
			return QPixmap(":empty");
		case Hole::RedState:
			return QPixmap(":red");
		case Hole::BlueState:
			return QPixmap(":blue");
		case Hole::SelectableState:
			return QPixmap(":selectable");
		default:
			return QPixmap();
	}
}

void Hole::updateHole (State state) {
	this->setIcon(Hole::stateToPixmap(state));
}
