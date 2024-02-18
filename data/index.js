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

            var table = document.getElementById("searchTable");

            var rows = table.getElementsByTagName("tr");
            // Start from the end to avoid issues with indexes changing as rows are removed
            for (var i = rows.length - 1; i > 0; i--) {
                // Remove all rows except the first one (header)
                table.deleteRow(i);
            }

            var tbody = table.querySelector("tbody");

            data.results.forEach(element => {
                let row = tbody.insertRow();

                var idCell = row.insertCell(0);
                idCell.textContent = element.containerId;

                var nameCell = row.insertCell(1);
                nameCell.textContent = element.name;

                var qtyCell = row.insertCell(2);
                qtyCell.textContent = element.qty;

                var illCell = row.insertCell(3);
                var illButton = document.createElement("button");
                illButton.textContent = "Illuminate";
                illButton.addEventListener("click", function() {
                    console.log("illuminating:", element.containerId);
                    let urlWithQueries = "/illuminate?cid=" + element.containerId;
                    fetch(urlWithQueries, {method: "POST"})
                });
                illCell.appendChild(illButton);
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

            var table = document.getElementById("listTable");

            var rows = table.getElementsByTagName("tr");
            // Start from the end to avoid issues with indexes changing as rows are removed
            for (var i = rows.length - 1; i > 0; i--) {
                // Remove all rows except the first one (header)
                table.deleteRow(i);
            }

            var tbody = table.querySelector("tbody");

            data.results.forEach(element => {
                let row = tbody.insertRow();

                var idCell = row.insertCell(0);
                idCell.textContent = element.containerId;

                var nameCell = row.insertCell(1);
                nameCell.textContent = element.name;

                var qtyCell = row.insertCell(2);
                qtyCell.textContent = element.qty;

                var illCell = row.insertCell(3);
                var illButton = document.createElement("button");
                illButton.textContent = "Illuminate";
                illButton.addEventListener("click", function() {
                    console.log("illuminating:", element.containerId);
                    let urlWithQueries = "/illuminate?cid=" + element.containerId;
                    fetch(urlWithQueries, {method: "POST"})
                });
                illCell.appendChild(illButton);
            });
        })
        .catch(error => {
            console.log("error listing:", error);
        });
});