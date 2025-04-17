const searchBox = document.getElementById("searchBox");
const suggestionsContainer = document.getElementById("suggestions");
const newWordInput = document.getElementById("newWord");
const addWordBtn = document.getElementById("addWordBtn");
const addStatus = document.getElementById("addStatus");
const addWordWrapper = document.querySelector('.add-word-wrapper');

let currentSuggestions = [];
let selectedSuggestionIndex = -1;

// Add word function
async function addNewWord() {
    const word = newWordInput.value.trim().toLowerCase();
    
    if (!word) {
        addStatus.textContent = "Please enter a word";
        addStatus.style.color = "red";
        return;
    }

    const response = await fetch(`/add-word?word=${encodeURIComponent(word)}`, {
        method: 'POST'
    });

    if (response.ok) {
        const result = await response.json();
        
        if (result.status === "word_added") {
            addStatus.textContent = "Word added successfully!";
            addStatus.style.color = "green";
            newWordInput.value = "";
            setTimeout(() => { addStatus.textContent = ""; }, 3000);
        } else {
            addStatus.textContent = `Failed to add word: ${result.message || "Unknown reason"}`;
            addStatus.style.color = "red";
        }
    } else {
        addStatus.textContent = `Error: ${response.status} ${response.statusText}`;
        addStatus.style.color = "red";
    }
}

addWordBtn.addEventListener("click", addNewWord);

newWordInput.addEventListener("keypress", (event) => {
    if (event.key === "Enter") {
        event.preventDefault();
        addNewWord();
    }
});

// Autocomplete functionality
searchBox.addEventListener("input", async () => {
    const query = searchBox.value.trim();
    const lastWord = query.split(" ").pop();
    selectedSuggestionIndex = -1;
    
    if (lastWord.length === 0) {
        suggestionsContainer.style.display = "none";
        currentSuggestions = [];
        if (addWordWrapper) addWordWrapper.style.display = "flex";
        // Hide search time if present
        const timeDisplay = document.getElementById("searchTime");
        if (timeDisplay) timeDisplay.style.display = "none";
        return;
    }

    try {
        const response = await fetch(`/suggestions?query=${encodeURIComponent(lastWord)}`);
        
        if (response.ok) {
            const result = await response.json();
            currentSuggestions = result.suggestions || [];
            suggestionsContainer.innerHTML = "";

            let timeDisplay = document.getElementById("searchTime");
            if (!timeDisplay) {
                timeDisplay = document.createElement("div");
                timeDisplay.id = "searchTime";
                timeDisplay.style.marginTop = "8px";
                timeDisplay.style.fontSize = "0.95em";
                timeDisplay.style.color = "#888";
                searchBox.parentNode.appendChild(timeDisplay);
            }
            timeDisplay.style.display = "block";
            timeDisplay.textContent = `Backend search time: ${result.search_time_us} μs`;

            if (currentSuggestions.length > 0) {
                suggestionsContainer.style.display = "block";
                if (addWordWrapper) addWordWrapper.style.display = "none";
                
                currentSuggestions.forEach((suggestion, index) => {
                    const suggestionItem = document.createElement("div");
                    suggestionItem.classList.add("suggestion-item");
                    
                    const fullQuery = query.replace(/(\S+)$/, suggestion);
                    suggestionItem.textContent = fullQuery;
                    
                    suggestionItem.onclick = () => {
                        searchBox.value = fullQuery + " ";
                        suggestionsContainer.style.display = "none";
                        currentSuggestions = [];
                        if (addWordWrapper) addWordWrapper.style.display = "flex";
                        timeDisplay.style.display = "none";
                    };
                    
                    suggestionsContainer.appendChild(suggestionItem);
                });
            } else {
                suggestionsContainer.style.display = "none";
                currentSuggestions = [];
                if (addWordWrapper) addWordWrapper.style.display = "flex";
            }
        }
    } catch (error) {
        console.error("Error fetching suggestions:", error);
        if (addWordWrapper) addWordWrapper.style.display = "flex";
    }
});

// Keyboard navigation
searchBox.addEventListener("keydown", (event) => {
    if (event.key === "Tab") {
        event.preventDefault();
        if (currentSuggestions.length > 0) {
            const query = searchBox.value.trim();
            const selectedSuggestion = currentSuggestions[selectedSuggestionIndex !== -1 ? 
                selectedSuggestionIndex : 0];
            const fullQuery = query.replace(/(\S+)$/, selectedSuggestion);
            searchBox.value = fullQuery + " ";
            suggestionsContainer.style.display = "none";
            currentSuggestions = [];
            selectedSuggestionIndex = -1;
            if (addWordWrapper) addWordWrapper.style.display = "flex";
        }
    }
    else if (event.key === "ArrowDown" || event.key === "ArrowUp") {
        event.preventDefault();
        if (currentSuggestions.length > 0) {
            if (event.key === "ArrowDown") {
                selectedSuggestionIndex = (selectedSuggestionIndex + 1) % currentSuggestions.length;
            } else {
                selectedSuggestionIndex = selectedSuggestionIndex <= 0 ? 
                    currentSuggestions.length - 1 : selectedSuggestionIndex - 1;
            }
            
            const suggestionItems = suggestionsContainer.getElementsByClassName("suggestion-item");
            Array.from(suggestionItems).forEach((item, index) => {
                if (index === selectedSuggestionIndex) {
                    item.classList.add("selected");
                    searchBox.value = item.textContent;
                } else {
                    item.classList.remove("selected");
                }
            });
        }
    }
});

document.addEventListener("click", (event) => {
    if (event.target !== searchBox) {
        suggestionsContainer.style.display = "none";
        currentSuggestions = [];
        selectedSuggestionIndex = -1;
        if (addWordWrapper) addWordWrapper.style.display = "flex";
    }
});
