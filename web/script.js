let isExpanded = false;
let isAddExpanded = false;

function expandSearch() {
    let container = document.getElementById("searchContainer");
    let input = document.getElementById("searchBox");

    if (!isExpanded) {
        container.classList.add("search-expanded");
        input.style.display = "block";
        input.focus();
    } else {
        container.classList.remove("search-expanded");
        input.style.display = "none";
    }
    isExpanded = !isExpanded;
}

function expandAdd() {
    let container = document.getElementById("addContainer");
    let input = document.getElementById("addBox");

    if (!isAddExpanded) {
        container.classList.add("search-expanded");
        input.style.display = "block";
        input.focus();
    } else {
        container.classList.remove("search-expanded");
        input.style.display = "none";
    }
    isAddExpanded = !isAddExpanded;
}

const suggestionsList = ["React.js", "Vue.js", "Angular", "Bootstrap", "Tailwind", "OpenCV", "Machine Learning", "AI", "JavaScript", "Python"];

function showSuggestions() {
    let input = document.getElementById("searchBox").value.toLowerCase();
    let dropdown = document.getElementById("suggestions");
    dropdown.innerHTML = ""; 

    if (input.length === 0) {
        dropdown.style.display = "none";
        return;
    }

    let filtered = suggestionsList.filter(item => item.toLowerCase().includes(input));
    
    if (filtered.length > 0) {
        dropdown.style.display = "block";
        filtered.forEach(item => {
            let listItem = document.createElement("li");
            listItem.innerHTML = `<a class="dropdown-item text-light" href="#">${item}</a>`;
            listItem.onclick = () => {
                document.getElementById("searchBox").value = item;
                dropdown.style.display = "none";
            };
            dropdown.appendChild(listItem);
        });
    } else {
        dropdown.style.display = "none";
    }
}
