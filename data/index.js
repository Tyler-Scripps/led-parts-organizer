document.getElementById("nameSearch").addEventListener("click", function() {
    let name = document.getElementById("nameSearchField").value;
    if (name.length == 0) {
        return;
    }
    let urlWithQueries = "/search?searchString=" + name;
    fetch(urlWithQueries)
        .then(response => {
            console.log("response to search:")
            console.log(response)
            return response.json();
        })
        .then(data => {
            console.log("found:");
            console.log(data.results);

            clearTable();

            data.results.forEach(element => {
                addPartToTable(element.containerId, element.name, element.qty);
            });
        })
        .catch(error => {
            console.log("error searching by name:", error);
        });
});

document.getElementById("addComponent").addEventListener("click", function() {
    let name = document.getElementById("nameAdd").value;
    let qty = document.getElementById("qtyAdd").value;
    let x = document.getElementById("xAdd").value;
    let y = document.getElementById("yAdd").value;
    if (name.length == 0 || qty.length == 0 || x.length == 0 || y.length == 0) {
        return;
    }
    let urlWithQueries = "/add?newQty=" + qty + "&name=" + name + "&x=" + x + "&y=" + y;
    console.log("requesting:", urlWithQueries)
    fetch(urlWithQueries, {method: "POST"})
        .then(response => {
            console.log(response.body);
        })
        .catch(error => {
            console.log("error adding:", error);
        });
});

document.getElementById("offBut").addEventListener("click", function() {
    fetch("/alloff", {method: "POST"})
        .then(response => {
            console.log(response.body);
        })
        .catch(error => {
            console.log("error turniong off leds:", error);
        });
});

document.getElementById("listBut").addEventListener("click", function() {
    fetch("/list")
        .then(response => {
            console.log("response to list:")
            console.log(response)
            return response.json();
        })
        .then(data => {
            console.log("got list:");
            console.log(data.results);

            clearTable();

            data.results.forEach(element => {
                addPartToTable(element.containerId, element.name, element.qty);
            });
        })
        .catch(error => {
            console.log("error listing:", error);
        });
});

document.getElementById("csvButton").addEventListener("click", function() {
    var input = document.getElementById('csvInput');
    if (input.files.length == 1) {
        var file = input.files[0];
        var reader = new FileReader();
        reader.onload = function(event) {
            var csvContent = event.target.result;
            var lines = csvContent.split('\n');
            for (var i = 1; i < lines.length; i++) {
                var line = lines[i];
                if (line.length > 0) {
                    var params = line.trim().split(',');
                    let urlWithQueries = "/add?cid=" + params[0] + "&name=" + params[1] + "&newQty=" + params[2] + "&lednum=" + params[3];
                    fetch(urlWithQueries, {method: "POST"});
                }
            }
        }
        reader.readAsText(file);
    } else {
        console.log("wrong number of inputs:", input.files.length);
    }
});

function clearTable() {
    var table = document.getElementById("resultsTable");

    var rows = table.getElementsByTagName("tr");
    // Start from the end to avoid issues with indexes changing as rows are removed
    for (var i = rows.length - 1; i > 0; i--) {
        // Remove all rows except the first one (header)
        table.deleteRow(i);
    }
}

function addPartToTable(id, name, qty) {
    var table = document.getElementById("resultsTable");
    var tbody = table.querySelector("tbody");

    let row = tbody.insertRow();

    var idCell = row.insertCell(0);
    idCell.textContent = id;

    var nameCell = row.insertCell(1);
    nameCell.textContent = name;

    var qtyCell = row.insertCell(2);
    var qtyInput = document.createElement("input");
    qtyInput.type = "number";
    qtyInput.value = qty;
    qtyInput.addEventListener("input", function(event) {
        let urlWithQueries = "/setqty?cid=" + id + "&qty=" + event.target.value;
        fetch(urlWithQueries, {method: "POST"})
            .then(response => {
                console.log(response.body);
            })
            .catch(error => {
                console.log("error setting qty:", error);
            });
    });
    qtyCell.appendChild(qtyInput);

    var illCell = row.insertCell(3);
    var illButton = document.createElement("button");
    illButton.textContent = "Illuminate";
    illButton.addEventListener("click", function() {
        console.log("illuminating:", id);
        let urlWithQueries = "/illuminate?cid=" + id;
        fetch(urlWithQueries, {method: "POST"})
    });
    illCell.appendChild(illButton);

    var delCell = row.insertCell(4);
    var delButton = document.createElement("button");
    delButton.textContent = "Delete";
    delButton.addEventListener("click", function() {
        console.log("deleting:", id);
        let urlWithQueries = "/delete?cid=" + id;
        fetch(urlWithQueries, {method: "POST"});
    });
    delCell.appendChild(delButton);
}