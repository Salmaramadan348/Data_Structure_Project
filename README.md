CSE331: Data Structures and Algorithms
=====================================

Functionalities
===============

Basic Features
--------------

1) XML Consistency Check
   - Detect inconsistencies such as missing or mismatched tags.
   - Visualize errors and provide automated fixes.

2) Formatting (Prettifying)
   - Format XML files using proper indentation to improve readability.

3) Conversion to JSON
   - Convert XML files into JSON format for use with JavaScript-based tools.

4) Minification
   - Remove unnecessary spaces and newlines to reduce file size.

5) Compression and Decompression
   - Compress XML files using a custom compression algorithm.
   - Decompress files to restore the original XML structure.


Advanced Features
-----------------

6) Graph Representation of XML Data
   - Represent users in a social network as a graph.
   - Visualize relationships such as followers and posts.
   - Save the graph as an image.

7) Network Analysis
   - Identify the most influential and most active users.
   - Suggest users to follow based on mutual connections.

8) Post Search
   - Search for posts containing specific words or topics.

9) Graph Visualization
   - Use external libraries or tools to display the social network graphically.
  
GUI Features
============

🗂️ File Input:
--------------
The user can load an XML file either by browsing the file system or by directly entering the content into the application.

🎛️ Operations Control:
----------------------
All supported operations (verification, formatting, conversion, minification, etc.) are triggered through dedicated buttons without requiring command-line interaction.

📄 Output Display:
-----------------
The result of each operation is displayed in a read-only text area, with an option to save the output to a file.


Command-Line Operations
=======================

Check Consistency:
------------------
XML_Editor verify -i sample.xml  
Validate the XML file and report any missing or mismatched tags.

--------------------------------------------------

Format XML (Prettify):
----------------------
XML_Editor formatt -i sample.xml -o output.xml  
Read the XML file, build an internal tree representation, and output a well-indented formatted XML file.

--------------------------------------------------

Convert to JSON:
----------------
XML_Editor tojson -i sample.xml -o output.json  
Convert the XML structure into JSON format and save the result.

--------------------------------------------------

Minify XML:
-----------
XML_Editor minify -i sample.xml -o output.xml  
Remove unnecessary whitespaces and newlines to reduce file size.

--------------------------------------------------

Compress XML:
-------------
XML_Editor compress -i sample.xml -o output.comp  
Compress the XML file using a custom compression algorithm.

--------------------------------------------------

Decompress XML:
---------------
XML_Editor decompress -i sample.comp -o output.xml  
Restore the original XML file from the compressed format.

--------------------------------------------------

Most Active Users:
------------------
XML_Editor most_active -i sample.xml  
Analyze the XML graph and display the most active users.

--------------------------------------------------

Most Influential Users:
-----------------------
XML_Editor most_influencer -i sample.xml  
Identify users with the highest influence based on follower relationships.

--------------------------------------------------

Post Search by Word:
--------------------
XML_Editor search -w word -i sample.xml  
Search for posts containing a specific word.

--------------------------------------------------

Post Search by Topic:
---------------------
XML_Editor search -t topic -i sample.xml  
Search for posts related to a specific topic.

Why This Project?
=================
This project offers practical experience in:
- Parsing and handling XML and JSON files.
- Designing user-friendly graphical interfaces.
- Applying optimal data structures to solve real-world problems.

Summary
=======
A compact XML Editor integrating validation, formatting, JSON conversion, minification,
compression, and basic social network analysis using efficient data structures.
