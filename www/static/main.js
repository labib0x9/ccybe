
const gameState = {
    board: Array(9).fill().map(() => Array(9).fill('')),
    smallBoardStatus: Array(9).fill(''),
    currentPlayer: 'X',
    nextBoard: -1,
    gameStatus: 'not-started', 
    winningLines: [
        [0, 1, 2], [3, 4, 5], [6, 7, 8],
        [0, 3, 6], [1, 4, 7], [2, 5, 8],
        [0, 4, 8], [2, 4, 6]
    ],
    stats: {
        humanWins: 0,
        agentWins: 0,
        draws: 0,
        timerWins: 0
    },
    recentMoves: [],
    moveCount: {
        human: 0,
        agent: 0
    },
    firstPlayer: 'human',
    difficulty: 4,
    timerEnabled: true, 
    timerDuration: 30,
    humanTimeLeft: 30,
    aiTimeLeft: 30,
    timerInterval: null,
    gameStarted: false 
};

const gameBoardElement = document.getElementById('game-board');
const statusMessageElement = document.getElementById('status-message');
const humanIndicator = document.getElementById('human-indicator');
const aiIndicator = document.getElementById('ai-indicator');
const humanIndicatorText = humanIndicator.querySelector('.indicator-text');
const aiIndicatorText = aiIndicator.querySelector('.indicator-text');

const humanTimerElement = document.getElementById('human-timer');
const aiTimerElement = document.getElementById('ai-timer');
const humanTimerValue = document.getElementById('human-timer-value');
const aiTimerValue = document.getElementById('ai-timer-value');

const gameOverlay = document.getElementById('game-overlay');
const resultIcon = document.getElementById('result-icon');
const resultTitle = document.getElementById('result-title');
const resultMessage = document.getElementById('result-message');
const playAgainBtn = document.getElementById('play-again-btn');
const overlayCloseBtn = document.getElementById('overlay-close-btn');

const restartButton = document.getElementById('restart-btn');
const difficultyToggle = document.getElementById('difficulty-toggle');
const firstPlayerToggle = document.getElementById('first-player-toggle');
const timerDurationSelect = document.getElementById('timer-duration');
const timerToggle = document.getElementById('timer-toggle');
const timerDurationContainer = document.getElementById('timer-duration-container');
const humanWinsElement = document.getElementById('human-wins');
const agentWinsElement = document.getElementById('agent-wins');
const drawsElement = document.getElementById('draws');
const timerWinsElement = document.getElementById('timer-wins');
const movesListElement = document.getElementById('moves-list');

function startTimer() {
    if (!gameState.timerEnabled) return;
    
    clearInterval(gameState.timerInterval);
    
    gameState.humanTimeLeft = gameState.timerDuration;
    gameState.aiTimeLeft = gameState.timerDuration;
    
    gameState.timerInterval = setInterval(() => {
        if (gameState.gameStatus !== 'ongoing') {
            clearInterval(gameState.timerInterval);
            return;
        }
        
        if (gameState.currentPlayer === 'X') {
            gameState.humanTimeLeft--;
            updateTimerDisplay('human', gameState.humanTimeLeft);
            
            if (gameState.humanTimeLeft <= 0) {
                timeRanOut('human');
            }
        } else {
            gameState.aiTimeLeft--;
            updateTimerDisplay('ai', gameState.aiTimeLeft);
            
            if (gameState.aiTimeLeft <= 0) {
                timeRanOut('ai');
            }
        }
    }, 1000);
}

function updateTimerDisplay(player, timeLeft) {
    if (!gameState.timerEnabled) return;
    
    const timerElement = player === 'human' ? humanTimerValue : aiTimerValue;
    const timeLeftFormatted = `${timeLeft}s`;
    
    timerElement.textContent = timeLeftFormatted;
    
    if (timeLeft <= 5) {
        timerElement.className = 'timer-value critical';
    } else if (timeLeft <= 10) {
        timerElement.className = 'timer-value warning';
    } else {
        timerElement.className = 'timer-value normal';
    }
}

function showTimer(player) {
    if (!gameState.timerEnabled) return;
    
    if (player === 'human') {
        humanTimerElement.classList.remove('hidden');
        humanTimerElement.classList.add('visible');
        aiTimerElement.classList.remove('visible');
        aiTimerElement.classList.add('hidden');
    } else {
        aiTimerElement.classList.remove('hidden');
        aiTimerElement.classList.add('visible');
        humanTimerElement.classList.remove('visible');
        humanTimerElement.classList.add('hidden');
    }
}

function hideTimers() {
    if (!gameState.timerEnabled) return;
    
    humanTimerElement.classList.remove('visible');
    humanTimerElement.classList.add('hidden');
    aiTimerElement.classList.remove('visible');
    aiTimerElement.classList.add('hidden');
}

function timeRanOut(player) {
    if (!gameState.timerEnabled) return;
    
    clearInterval(gameState.timerInterval);
    
    gameState.stats.timerWins++;
    
    if (player === 'human') {
        gameState.gameStatus = 'O';
        gameState.stats.agentWins++;
        resultIcon.textContent = '⏰🤖';
        resultIcon.style.color = '#f72585';
        resultTitle.textContent = 'Time Out!';
        resultTitle.style.color = '#f72585';
        resultMessage.textContent = 'You ran out of time! Agent wins by timeout!';
        resultMessage.style.color = '#f72585';
    } else {
        gameState.gameStatus = 'X';
        gameState.stats.humanWins++;
        resultIcon.textContent = '⏰🏆';
        resultIcon.style.color = '#4cc9f0';
        resultTitle.textContent = 'Time Out!';
        resultTitle.style.color = '#4cc9f0';
        resultMessage.textContent = 'Agent ran out of time! You win by timeout!';
        resultMessage.style.color = '#4cc9f0';
    }
    
    updateStatsDisplay();
    showGameOverOverlay();
}

function initToggles() {
    const timerOptions = timerToggle.querySelectorAll('.toggle-option');
    timerOptions.forEach(option => {
        option.addEventListener('click', () => {
            timerOptions.forEach(opt => opt.classList.remove('active'));
            option.classList.add('active');
            gameState.timerEnabled = option.dataset.timer === 'with';
            
            if (gameState.timerEnabled) {
                timerDurationContainer.style.display = 'flex';
                updateTimerDisplay('human', gameState.humanTimeLeft);
                updateTimerDisplay('ai', gameState.aiTimeLeft);
            } else {
                timerDurationContainer.style.display = 'none';
                hideTimers();
            }
            
            if (gameState.gameStatus === 'ongoing') {
                updateStatusMessage();
            }
        });
    });

    const difficultyOptions = difficultyToggle.querySelectorAll('.toggle-option');
    difficultyOptions.forEach(option => {
        option.addEventListener('click', () => {
            difficultyOptions.forEach(opt => opt.classList.remove('active'));
            option.classList.add('active');
            gameState.difficulty = parseInt(option.dataset.difficulty);
        });
    });

    const playerOptions = firstPlayerToggle.querySelectorAll('.toggle-option');
    playerOptions.forEach(option => {
        option.addEventListener('click', () => {
            playerOptions.forEach(opt => opt.classList.remove('active'));
            option.classList.add('active');
            gameState.firstPlayer = option.dataset.player;
        });
    });
    
    timerDurationSelect.addEventListener('change', () => {
        gameState.timerDuration = parseInt(timerDurationSelect.value);
        gameState.humanTimeLeft = gameState.timerDuration;
        gameState.aiTimeLeft = gameState.timerDuration;
        updateTimerDisplay('human', gameState.humanTimeLeft);
        updateTimerDisplay('ai', gameState.aiTimeLeft);
    });
}

function updateStatsDisplay() {
    humanWinsElement.textContent = gameState.stats.humanWins;
    agentWinsElement.textContent = gameState.stats.agentWins;
    drawsElement.textContent = gameState.stats.draws;
    timerWinsElement.textContent = gameState.stats.timerWins;
}

function addRecentMove(boardIndex, cellIndex, player, isHuman) {
    const boardNumber = boardIndex + 1;
    
    const row = Math.floor(cellIndex / 3);
    const col = cellIndex % 3;
    const rowNumber = row + 1; 
    const colNumber = col + 1; 
    
    const playerType = isHuman ? 'H' : 'A';
    
    if (isHuman) {
        gameState.moveCount.human++;
    } else {
        gameState.moveCount.agent++;
    }
    
    const moveNumber = isHuman ? gameState.moveCount.human : gameState.moveCount.agent;
    
    const move = {
        player: isHuman ? 'Human' : 'Agent',
        playerType: playerType,
        moveNumber: moveNumber,
        notation: `${playerType}:${boardNumber}(${rowNumber},${colNumber})`,
        boardNumber: boardNumber,
        rowNumber: rowNumber,
        colNumber: colNumber,
        isHuman: isHuman,
        timestamp: new Date()
    };
    
    gameState.recentMoves.unshift(move);
    
    if (gameState.recentMoves.length > 8) {
        gameState.recentMoves.pop();
    }
    
    updateMovesDisplay();
}

function updateMovesDisplay() {
    movesListElement.innerHTML = '';
    
    if (gameState.recentMoves.length === 0) {
        const emptyMessage = gameState.gameStarted ? 
            'No moves yet. Make the first move!' : 
            'No moves yet. Click "Restart Game" to start!';
        movesListElement.innerHTML = `<div class="empty-moves">${emptyMessage}</div>`;
        return;
    }
    
    gameState.recentMoves.forEach((move) => {
        const moveItem = document.createElement('li');
        moveItem.className = `move-item ${move.isHuman ? 'human' : 'agent'}`;
        
        const moveSpan = document.createElement('span');
        moveSpan.className = 'move-notation';
        moveSpan.textContent = move.notation;
        
        moveItem.appendChild(moveSpan);
        movesListElement.appendChild(moveItem);
    });
}

function updatePlayerIndicators() {
    if (gameState.gameStatus === 'not-started') {
        humanIndicator.classList.remove('active');
        aiIndicator.classList.remove('active');
        humanIndicatorText.textContent = 'Your Turn';
        aiIndicatorText.textContent = 'Disabled';
        humanIndicatorText.classList.remove('status-active');
        aiIndicatorText.classList.remove('status-active');
        hideTimers();
        return;
    }
    
    if (gameState.gameStatus !== 'ongoing') {
        humanIndicator.classList.remove('active');
        aiIndicator.classList.remove('active');
        humanIndicatorText.textContent = 'Game Over';
        aiIndicatorText.textContent = 'Game Over';
        humanIndicatorText.classList.remove('status-active');
        aiIndicatorText.classList.remove('status-active');
        hideTimers();
        return;
    }
    
    if (gameState.currentPlayer === 'X') {
        humanIndicator.classList.add('active');
        aiIndicator.classList.remove('active');
        humanIndicatorText.textContent = 'Your turn';
        aiIndicatorText.textContent = 'Disabled';
        humanIndicatorText.classList.add('status-active');
        aiIndicatorText.classList.remove('status-active');
        showTimer('human');
    } else {
        humanIndicator.classList.remove('active');
        aiIndicator.classList.add('active');
        humanIndicatorText.textContent = 'Disabled';
        aiIndicatorText.textContent = 'AI thinking..';
        humanIndicatorText.classList.remove('status-active');
        aiIndicatorText.classList.add('status-active');
        showTimer('ai');
    }
}

function initGame() {
    gameState.board = Array(9).fill().map(() => Array(9).fill(''));
    gameState.smallBoardStatus = Array(9).fill('');
    gameState.nextBoard = -1;
    gameState.gameStatus = 'ongoing';
    gameState.gameStarted = true;
    
    gameState.moveCount.human = 0;
    gameState.moveCount.agent = 0;
    
    gameState.timerDuration = parseInt(timerDurationSelect.value);
    gameState.humanTimeLeft = gameState.timerDuration;
    gameState.aiTimeLeft = gameState.timerDuration;
    
    if (gameState.firstPlayer === 'human') {
        gameState.currentPlayer = 'X';
    } else {
        gameState.currentPlayer = 'O';
    }
    
    gameState.recentMoves = [];
    
    hideGameOverOverlay();
    updateBoardUI();
    updatePlayerIndicators();
    updateStatusMessage();
    updateMovesDisplay();
    updateTimerDisplay('human', gameState.humanTimeLeft);
    updateTimerDisplay('ai', gameState.aiTimeLeft);
    
    startTimer();
    
    if (gameState.currentPlayer === 'O') {
        setTimeout(() => {
            if (gameState.gameStatus === 'ongoing') {
                makeAIMove();
            }
        }, 500);
    }
}

function updateBoardUI() {
    const cells = document.querySelectorAll('.cell');
    cells.forEach(cell => {
        cell.textContent = '';
        cell.classList.remove('human', 'ai', 'disabled');
        cell.style.pointerEvents = 'auto';
        
        const newCell = cell.cloneNode(true);
        cell.parentNode.replaceChild(newCell, cell);
    });
    
    const largeCells = document.querySelectorAll('.large-cell');
    largeCells.forEach((largeCell, boardIndex) => {
        largeCell.classList.remove('highlight-board', 'won-board', 'human', 'ai');
        largeCell.removeAttribute('data-winner');
        
        if (gameState.gameStatus === 'ongoing' && 
            (gameState.nextBoard === -1 || gameState.nextBoard === boardIndex)) {
            largeCell.classList.add('highlight-board');
        }
        
        if (gameState.smallBoardStatus[boardIndex] === 'X') {
            largeCell.classList.add('won-board', 'human');
            largeCell.setAttribute('data-winner', 'X');
        } else if (gameState.smallBoardStatus[boardIndex] === 'O') {
            largeCell.classList.add('won-board', 'ai');
            largeCell.setAttribute('data-winner', 'O');
        } else if (gameState.smallBoardStatus[boardIndex] === 'D') {
            largeCell.classList.add('won-board');
            largeCell.setAttribute('data-winner', 'D');
        }
    });
    
    for (let largeBoardIndex = 0; largeBoardIndex < 9; largeBoardIndex++) {
        for (let smallCellIndex = 0; smallCellIndex < 9; smallCellIndex++) {
            const cell = document.querySelector(`.cell[data-large-index="${largeBoardIndex}"][data-small-index="${smallCellIndex}"]`);
            
            if (!cell) continue;
            
            const cellValue = gameState.board[largeBoardIndex][smallCellIndex];
            
            if (cellValue === 'X') {
                cell.textContent = 'X';
                cell.classList.add('human');
            } else if (cellValue === 'O') {
                cell.textContent = 'O';
                cell.classList.add('ai');
            }
            
            if (gameState.gameStatus === 'ongoing' && 
                cellValue === '' && 
                gameState.smallBoardStatus[largeBoardIndex] === '' &&
                (gameState.nextBoard === -1 || gameState.nextBoard === largeBoardIndex)) {
                
                cell.addEventListener('click', () => handleCellClick(largeBoardIndex, smallCellIndex));
            } else {
                cell.classList.add('disabled');
                cell.style.pointerEvents = 'none';
            }
        }
    }
}

function setupInitialBoard() {
    gameState.board = Array(9).fill().map(() => Array(9).fill(''));
    gameState.smallBoardStatus = Array(9).fill('');
    gameState.nextBoard = -1;
    gameState.gameStatus = 'not-started';
    gameState.gameStarted = false;
    
    gameState.moveCount.human = 0;
    gameState.moveCount.agent = 0;
    
    gameState.recentMoves = [];
    
    hideGameOverOverlay();
    updateBoardUI();
    updatePlayerIndicators();
    updateStatusMessage();
    updateMovesDisplay();
    updateTimerDisplay('human', gameState.timerDuration);
    updateTimerDisplay('ai', gameState.timerDuration);
    hideTimers();
}

function updateStatusMessage() {
    if (gameState.gameStatus === 'not-started') {
        statusMessageElement.textContent = 'Click "Restart Game" to start playing!';
        statusMessageElement.style.color = '#aaa';
        statusMessageElement.classList.remove('ai-thinking');
        return;
    }
    
    if (gameState.gameStatus === 'ongoing') {
        if (gameState.currentPlayer === 'X') {
            if (gameState.timerEnabled) {
                statusMessageElement.textContent = gameState.nextBoard === -1 
                    ? `Your turn! ${gameState.humanTimeLeft}s remaining. Click on any available cell.` 
                    : `Your turn! ${gameState.humanTimeLeft}s remaining. You must play in board ${gameState.nextBoard + 1}.`;
            } else {
                statusMessageElement.textContent = gameState.nextBoard === -1 
                    ? `Your turn! Click on any available cell.` 
                    : `Your turn! You must play in board ${gameState.nextBoard + 1}.`;
            }
            statusMessageElement.style.color = '#4cc9f0';
            statusMessageElement.classList.remove('ai-thinking');
        } else {
            if (gameState.timerEnabled) {
                statusMessageElement.textContent = `Agent is analyzing the board... ${gameState.aiTimeLeft}s remaining.`;
            } else {
                statusMessageElement.textContent = `Agent is analyzing the board...`;
            }
            statusMessageElement.style.color = '#f72585';
            statusMessageElement.classList.add('ai-thinking');
        }
    } else {
        statusMessageElement.textContent = 'Game Over!';
    }
}

function handleCellClick(largeBoardIndex, smallCellIndex) {
    if (gameState.gameStatus !== 'ongoing' || gameState.currentPlayer !== 'X') {
        return;
    }
    
    if (!isValidMove(largeBoardIndex, smallCellIndex)) {
        return;
    }
    
    makeMove(largeBoardIndex, smallCellIndex, 'X', true);
    
    if (gameState.gameStatus === 'ongoing') {
        gameState.currentPlayer = 'O';
        updatePlayerIndicators();
        updateStatusMessage();
        
        setTimeout(makeAIMove, 500);
    } else {
        updatePlayerIndicators();
    }
}

function makeAIMove() {
    setTimeout(() => {
        const move = getBestMove(gameState.difficulty);
        
        if (move) {
            makeMove(move.boardIndex, move.cellIndex, 'O', false);
            
            if (gameState.gameStatus === 'ongoing') {
                gameState.currentPlayer = 'X';
                updatePlayerIndicators();
                updateStatusMessage();
            } else {
                updatePlayerIndicators();
            }
        }
    }, 100);
}

function makeMove(largeBoardIndex, smallCellIndex, player, isHuman) {
    gameState.board[largeBoardIndex][smallCellIndex] = player;
    
    addRecentMove(largeBoardIndex, smallCellIndex, player, isHuman);
    
    checkSmallBoardWin(largeBoardIndex);
    gameState.nextBoard = smallCellIndex;
    
    if (gameState.smallBoardStatus[gameState.nextBoard] !== '') {
        gameState.nextBoard = -1;
    }
    
    checkGameOver();
    updateBoardUI();
    updateStatusMessage();
    
    if (gameState.gameStatus === 'ongoing') {
        if (player === 'X') {
            gameState.aiTimeLeft = gameState.timerDuration;
            updateTimerDisplay('ai', gameState.aiTimeLeft);
        } else {
            gameState.humanTimeLeft = gameState.timerDuration;
            updateTimerDisplay('human', gameState.humanTimeLeft);
        }
    }
}

function isValidMove(largeBoardIndex, smallCellIndex) {
    if (gameState.gameStatus !== 'ongoing') {
        return false;
    }
    
    if (gameState.board[largeBoardIndex][smallCellIndex] !== '') {
        return false;
    }
    
    if (gameState.smallBoardStatus[largeBoardIndex] !== '') {
        return false;
    }
    
    if (gameState.nextBoard !== -1 && gameState.nextBoard !== largeBoardIndex) {
        return false;
    }
    
    return true;
}

function checkSmallBoardWin(boardIndex) {
    const board = gameState.board[boardIndex];
    
    for (const line of gameState.winningLines) {
        const [a, b, c] = line;
        if (board[a] && board[a] === board[b] && board[a] === board[c]) {
            gameState.smallBoardStatus[boardIndex] = board[a];
            return;
        }
    }
    
    if (!board.includes('')) {
        gameState.smallBoardStatus[boardIndex] = 'D';
    }
}

function checkGameOver() {
    for (const line of gameState.winningLines) {
        const [a, b, c] = line;
        if (gameState.smallBoardStatus[a] && 
            gameState.smallBoardStatus[a] !== 'D' &&
            gameState.smallBoardStatus[a] === gameState.smallBoardStatus[b] &&
            gameState.smallBoardStatus[a] === gameState.smallBoardStatus[c]) {
            gameState.gameStatus = gameState.smallBoardStatus[a];
            
            if (gameState.gameStatus === 'X') {
                gameState.stats.humanWins++;
            } else {
                gameState.stats.agentWins++;
            }
            updateStatsDisplay();
            
            showGameOverOverlay();
            return;
        }
    }
    
    if (gameState.smallBoardStatus.every(status => status !== '')) {
        gameState.gameStatus = 'draw';
        gameState.stats.draws++;
        updateStatsDisplay();
        showGameOverOverlay();
    }
}

function showGameOverOverlay() {
    clearInterval(gameState.timerInterval);
    
    const cells = document.querySelectorAll('.cell');
    cells.forEach(cell => {
        cell.classList.add('disabled');
        cell.style.pointerEvents = 'none';
    });
    
    if (gameState.gameStatus === 'X') {
        resultIcon.textContent = '🏆';
        resultIcon.style.color = '#4cc9f0';
        resultTitle.textContent = 'Victory!';
        resultTitle.style.color = '#4cc9f0';
        resultMessage.textContent = 'Congratulations! You defeated the Agent!';
        resultMessage.style.color = '#4cc9f0';
    } else if (gameState.gameStatus === 'O') {
        resultIcon.textContent = '🤖';
        resultIcon.style.color = '#f72585';
        resultTitle.textContent = 'Defeat!';
        resultTitle.style.color = '#f72585';
        resultMessage.textContent = 'The Agent wins! Better luck next time!';
        resultMessage.style.color = '#f72585';
    } else if (gameState.gameStatus === 'draw') {
        resultIcon.textContent = '🤝';
        resultIcon.style.color = '#ffd700';
        resultTitle.textContent = 'Draw Game!';
        resultTitle.style.color = '#ffd700';
        resultMessage.textContent = 'The game ended in a draw! Well played!';
        resultMessage.style.color = '#ffd700';
    }
    
    setTimeout(() => {
        gameOverlay.classList.add('active');
    }, 500);
}

function hideGameOverOverlay() {
    gameOverlay.classList.remove('active');
    
    resultIcon.style.color = '';
    resultTitle.style.color = '';
    resultMessage.style.color = '';
}

function getBestMove(depth) {
    let bestScore = -Infinity;
    let bestMove = null;
    
    const validMoves = getAllValidMoves('O');
    
    for (const move of validMoves) {
        const tempBoard = JSON.parse(JSON.stringify(gameState.board));
        const tempSmallBoardStatus = [...gameState.smallBoardStatus];
        const tempNextBoard = gameState.nextBoard;
        const tempGameStatus = gameState.gameStatus;
        
        gameState.board[move.boardIndex][move.cellIndex] = 'O';
        checkSmallBoardWin(move.boardIndex);
        const oldNextBoard = gameState.nextBoard;
        gameState.nextBoard = move.cellIndex;
        if (gameState.smallBoardStatus[gameState.nextBoard] !== '') {
            gameState.nextBoard = -1;
        }
        
        const score = minimax(depth - 1, -Infinity, Infinity, false);
        
        gameState.board = tempBoard;
        gameState.smallBoardStatus = tempSmallBoardStatus;
        gameState.nextBoard = oldNextBoard;
        gameState.gameStatus = tempGameStatus;
        
        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }
    
    return bestMove;
}

function minimax(depth, alpha, beta, isMaximizing) {
    if (depth === 0) {
        return evaluateBoard();
    }
    
    const gameResult = checkTerminalState();
    if (gameResult !== null) {
        if (gameResult === 'X') return -1000;
        if (gameResult === 'O') return 1000;
        if (gameResult === 'draw') return 0;
    }
    
    if (isMaximizing) {
        let maxScore = -Infinity;
        const validMoves = getAllValidMoves('O');
        
        for (const move of validMoves) {
            const tempBoard = JSON.parse(JSON.stringify(gameState.board));
            const tempSmallBoardStatus = [...gameState.smallBoardStatus];
            const tempNextBoard = gameState.nextBoard;
            
            gameState.board[move.boardIndex][move.cellIndex] = 'O';
            checkSmallBoardWin(move.boardIndex);
            const oldNextBoard = gameState.nextBoard;
            gameState.nextBoard = move.cellIndex;
            if (gameState.smallBoardStatus[gameState.nextBoard] !== '') {
                gameState.nextBoard = -1;
            }
            
            const score = minimax(depth - 1, alpha, beta, false);
            maxScore = Math.max(maxScore, score);
            
            
            gameState.board = tempBoard;
            gameState.smallBoardStatus = tempSmallBoardStatus;
            gameState.nextBoard = tempNextBoard;
            
            alpha = Math.max(alpha, score);
            if (beta <= alpha) {
                break;
            }
        }
        
        return maxScore;
    } else {
        let minScore = Infinity;
        const validMoves = getAllValidMoves('X');
        
        for (const move of validMoves) {
            const tempBoard = JSON.parse(JSON.stringify(gameState.board));
            const tempSmallBoardStatus = [...gameState.smallBoardStatus];
            const tempNextBoard = gameState.nextBoard;
            
            
            gameState.board[move.boardIndex][move.cellIndex] = 'X';
            checkSmallBoardWin(move.boardIndex);
            const oldNextBoard = gameState.nextBoard;
            gameState.nextBoard = move.cellIndex;
            if (gameState.smallBoardStatus[gameState.nextBoard] !== '') {
                gameState.nextBoard = -1;
            }
            
            const score = minimax(depth - 1, alpha, beta, true);
            minScore = Math.min(minScore, score);
            
            
            gameState.board = tempBoard;
            gameState.smallBoardStatus = tempSmallBoardStatus;
            gameState.nextBoard = tempNextBoard;
            
            beta = Math.min(beta, score);
            if (beta <= alpha) {
                break;
            }
        }
        
        return minScore;
    }
}

function checkTerminalState() {
    
    for (const line of gameState.winningLines) {
        const [a, b, c] = line;
        if (gameState.smallBoardStatus[a] && gameState.smallBoardStatus[a] !== 'D' &&
            gameState.smallBoardStatus[a] === gameState.smallBoardStatus[b] &&
            gameState.smallBoardStatus[a] === gameState.smallBoardStatus[c]) {
            return gameState.smallBoardStatus[a];
        }
    }
    
    
    if (gameState.smallBoardStatus.every(status => status !== '')) {
        return 'draw';
    }
    
    return null;
}

function getAllValidMoves(player) {
    const moves = [];
    
    let boardsToCheck = [];
    if (gameState.nextBoard === -1) {
        boardsToCheck = [...Array(9).keys()].filter(i => gameState.smallBoardStatus[i] === '');
    } else {
        if (gameState.smallBoardStatus[gameState.nextBoard] === '') {
            boardsToCheck = [gameState.nextBoard];
        } else {
            boardsToCheck = [...Array(9).keys()].filter(i => gameState.smallBoardStatus[i] === '');
        }
    }
    
    for (const boardIndex of boardsToCheck) {
        for (let cellIndex = 0; cellIndex < 9; cellIndex++) {
            if (gameState.board[boardIndex][cellIndex] === '') {
                moves.push({boardIndex, cellIndex});
            }
        }
    }
    
    return moves;
}

function evaluateBoard() {
    let score = 0;
    
    
    for (let i = 0; i < 9; i++) {
        if (gameState.smallBoardStatus[i] === 'O') {
            score += 100;
        } else if (gameState.smallBoardStatus[i] === 'X') {
            score -= 100;
        } else if (gameState.smallBoardStatus[i] === '') {
            const boardScore = evaluateSmallBoard(i);
            score += boardScore;
        }
    }
    
    
    const largeBoardScore = evaluateLargeBoard();
    score += largeBoardScore * 10;
    
    return score;
}

function evaluateSmallBoard(boardIndex) {
    const board = gameState.board[boardIndex];
    let score = 0;
    
    for (const line of gameState.winningLines) {
        const [a, b, c] = line;
        const lineValues = [board[a], board[b], board[c]];
        
        const countO = lineValues.filter(cell => cell === 'O').length;
        const countX = lineValues.filter(cell => cell === 'X').length;
        
        if (countO > 0 && countX === 0) {
            score += countO * 10;
        } else if (countX > 0 && countO === 0) {
            score -= countX * 10;
        }
    }
    
    return score;
}

function evaluateLargeBoard() {
    let score = 0;
    
    for (const line of gameState.winningLines) {
        const [a, b, c] = line;
        const lineValues = [
            gameState.smallBoardStatus[a],
            gameState.smallBoardStatus[b],
            gameState.smallBoardStatus[c]
        ];
        
        const countO = lineValues.filter(status => status === 'O').length;
        const countX = lineValues.filter(status => status === 'X').length;
        const countEmpty = lineValues.filter(status => status === '').length;
        
        if (countO > 0 && countX === 0) {
            score += countO * 25;
        } else if (countX > 0 && countO === 0) {
            score -= countX * 25;
        }
    }
    
    return score;
}

document.addEventListener('DOMContentLoaded', () => {
    initToggles();
    updateStatsDisplay();
    setupInitialBoard();
    
    restartButton.addEventListener('click', initGame);
    playAgainBtn.addEventListener('click', initGame);
    overlayCloseBtn.addEventListener('click', hideGameOverOverlay);
});