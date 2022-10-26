MongoLogPipe
============

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Brought to you by Dimitar Kunchev this program serves as pipe to a MongoDB collection. Every line sent to the program's stdin is timestamped and saved in the database. Useful for example to log from Apache CustomLog (with piped logging).

## Requirements
 - libmongoc, libbson, libconfig
 
## Build steps

1. Install libmongoc-dev, libbson-dev, libconfig-dev
2. mkdir build
3. cd build
4. cmake ..
5. make

... and you can run mongoLogPipe from the build directory. 

## Config options

Run the program with -h for the available options. You can configure the database connection string (DSN), datatabase name and collection name. Optionally you can tag the logs so you can have multiple instances piping data to the same collection and still have info what came from where.

## Config file

See the sample.cfg   

## Author & Copyright

Dimitar Kunchev

## License

This extension is licensed under the MIT License - see the `LICENSE` file for details

## Contributing

Pull requests and issues are more than welcome.