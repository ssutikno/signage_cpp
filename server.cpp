#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include "crow.h"

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")
    ([](const crow::request& req, crow::response& res){
        // Serve the HTML page
        std::ifstream file("upload.html");
        if (file) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            res.set_header("Content-Type", "text/html");
            res.write(buffer.str());
            res.end();
        } else {
            // Return an error if the HTML file is not found
            res.code = 404;
            res.body = "Error: HTML file not found";
        }
    });

    CROW_ROUTE(app, "/upload")
    .methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        // Get the uploaded files
        const auto files = req.get_multipart()["file"];
        if (files.empty()) {
            // Return an error if no files were uploaded
            res.code = 400;
            res.body = "Error: no files uploaded";
            return;
        }

        // Save each file to disk
        for (const auto& file : files) {
            std::string filename = file.filename;
            std::ifstream src(file.data, std::ios::binary);
            std::ofstream dst(filename, std::ios::binary);
            dst << src.rdbuf();
        }

        // Return a success message
        res.code = 200;
        res.body = "Files uploaded successfully";
    });

    CROW_ROUTE(app, "/file/<string>")
    ([](const crow::request& req, crow::response& res, std::string filename){
        // Serve the requested file
        std::ifstream file(filename, std::ios::binary);
        if (file) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            res.set_header("Content-Type", "application/octet-stream");
            res.write(buffer.str());
            res.end();
        } else {
            // Return an error if the file is not found
            res.code = 404;
            res.body = "Error: file not found";
        }
    });

    CROW_ROUTE(app, "/delete/<string>")
    ([](const crow::request& req, crow::response& res, std::string filename){
        // Delete the requested file
        if (std::remove(filename.c_str()) == 0) {
            // Return a success message if the file was deleted
            res.code = 200;
            res.body = "File deleted successfully";
        } else {
            // Return an error if the file could not be deleted
            res.code = 500;
            res.body = "Error: could not delete file";
        }
    });

    app.port(8080).run();
}
