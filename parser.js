const fs = require('fs');

try {
    const data = fs.readFileSync('./neh.data.txt', 'utf8');
    const content = data.split("\r\n\r\n");

    content.forEach((e, i) => {
        if(e.includes("data")) {
            let tmp = e.split("\r\n");
            let name = tmp[0];
            tmp.shift();
            let newData = tmp.join("\r\n");
            
            
            fs.writeFileSync(`./data${i/2}.txt`, newData);
            console.log(`File ${name}.txt written successfully with data:}`);
        }
    });
} catch (err) {
    console.error('Error reading or writing file:', err);
}
