import 'dart:convert';
import 'dart:typed_data';
import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';

void main() => runApp(MyApp());

String? playerName;

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Bluetooth Flutter App',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: MyHomePage(),
    );
  }
}

class MyHomePage extends StatefulWidget {
  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  BluetoothConnection? connection;
  int? elapsedTime;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      //appBar: AppBar(
      //  title: Text('Bluetooth Communication'),
     // ),
      body: Stack(
        children: <Widget>[
          // Background Image
          Image.asset(
            "assets/white.jpg", // Replace with the actual path
            width: double.infinity,
            height: double.infinity,
            fit: BoxFit.cover,
          ),

          Center(
            child: Column(
              mainAxisAlignment: MainAxisAlignment.center,
              children: <Widget>[
                Image.asset(
                  "assets/hamsa.jpg", // Replace with the actual path
                  width: 200, // Set the width as needed
                  height: 200, // Set the height as needed
                ),
                ElevatedButton(
                  onPressed: _connect,
                  child: Text('Find Device'),
                ),
                if (playerName != null) SizedBox(height: 20),
                if (playerName != null)
                  ElevatedButton(
                    onPressed: () {
                      Navigator.push(
                        context,
                        MaterialPageRoute(
                          builder: (context) => GameMenuScreen(
                            playerName!,
                            startGameCallback: () => _start_game(connection),
                          ),
                        ),
                      );
                    },
                    child: Text('Open Game Menu'),
                  ),
              ],
            ),
          ),
        ],
      ),
    );
  }

  void _connect() async {
    BluetoothDevice? selectedDevice = await Navigator.push(
      context,
      MaterialPageRoute(builder: (context) => DeviceListPage()),
    );

    if (selectedDevice != null) {
      try {
        connection = await BluetoothConnection.toAddress(selectedDevice.address);
        print('Connected to ${selectedDevice.name}');

        if (connection != null) {
          if (connection!.isConnected && connection!.input != null) {
            connection!.input!.listen((Uint8List data) {
              print('Data incoming: ${ascii.decode(data)}');
              String received = ascii.decode(data).trim();

              if (received.startsWith('end_game_time ')) {
                setState(() {
                  elapsedTime = int.tryParse(received.substring(14));
                });
              }
            }).onDone(() {
              // Handle what happens when the connection is closed
            });
          }
        }
      } catch (e) {
        print('Error connecting: $e');
      }
    }
  }
}

class DeviceListPage extends StatelessWidget {
  Future<BluetoothDevice?> _getBondedDevice(BuildContext context) async {
    List<BluetoothDevice> devices = [];
    try {
      devices = await FlutterBluetoothSerial.instance.getBondedDevices();
    } catch (e) {
      print('Error getting bonded devices: $e');
    }

    BluetoothDevice? selectedDevice = await Navigator.push(
      context,
      MaterialPageRoute(
        builder: (context) => EnterNameScreen(devices),
      ),
    );

    return selectedDevice;
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Select Device'),
      ),
      body: FutureBuilder<BluetoothDevice?>(
        future: _getBondedDevice(context),
        builder: (context, snapshot) {
          if (snapshot.connectionState == ConnectionState.waiting) {
            return Center(child: CircularProgressIndicator());
          } else if (snapshot.hasError) {
            return Center(child: Text('Error: ${snapshot.error}'));
          } else if (!snapshot.hasData || snapshot.data == null) {
            return Center(child: Text('No bonded devices found.'));
          } else {
            return Center(
              child: Text('Selected Device: ${snapshot.data!.name}'),
            );
          }
        },
      ),
    );
  }
}

class EnterNameScreen extends StatefulWidget {
  final List<BluetoothDevice> devices;

  EnterNameScreen(this.devices);

  @override
  _EnterNameScreenState createState() => _EnterNameScreenState();
}

class _EnterNameScreenState extends State<EnterNameScreen> {
  String? enteredName;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Enter Your Name'),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            TextField(
              onChanged: (value) {
                setState(() {
                  enteredName = value;
                });
              },
              decoration: InputDecoration(labelText: 'Enter Your Name'),
            ),
            SizedBox(height: 20),
            ElevatedButton(
              onPressed: () {
                setState(() {
                  playerName = enteredName;
                });
                Navigator.push(
                  context,
                  MaterialPageRoute(
                    builder: (context) => GameMenuScreen(
                      playerName!,
                      startGameCallback: () {},
                    ),
                  ),
                );
              },
              child: Text('Confirm Name'),
            ),
          ],
        ),
      ),
    );
  }
}

class GameMenuScreen extends StatelessWidget {
  final String playerName;
  final Function startGameCallback;

  GameMenuScreen(this.playerName, {required this.startGameCallback});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Hello $playerName'),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            ElevatedButton(
              onPressed: () {
                // Call the startGameCallback to start the game
                startGameCallback();
              },
              child: Text('Start Shimon Game'),
            ),
            ElevatedButton(
              onPressed: () {
                // Handle opening the settings screen here
              },
              child: Text('Settings'),
            ),
            ElevatedButton(
              onPressed: () {
                // Handle opening the top players screen here
              },
              child: Text('Top Players'),
            ),
          ],
        ),
      ),
    );
  }
}

void _start_game(BluetoothConnection? connection) {
  if (connection != null && connection.isConnected) {
    Uint8List data = Uint8List.fromList(utf8.encode('start_game'));
    connection.output.add(data);
    connection.output.allSent.then((_) {
      print('start_game');
    });
  } else {
    print('Not connected');
  }
}
