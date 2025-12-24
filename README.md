CSE331: Data Structures and Algorithms – XML Editor Project
====================
Functionalities
====================

Basic Features:
---------------
1) XML Consistency Check
   - Detect missing or mismatched tags.
   - Visualize errors and optionally fix them.

2) Formatting (Prettifying)
   - Proper indentation for readability.

3) Conversion to JSON
   - Convert XML files into JSON format.

4) Minification
   - Remove unnecessary spaces and newlines.

5) Compression & Decompression
   - Compress XML files efficiently.
   - Decompress files to restore original structure.

Advanced Features:
------------------
1) Graph Representation of XML Data
   - Represent users and relationships as a graph.
   - Visualize and save graphs as images.

2) Network Analysis
   - Identify the most active and most influential users.
   - Suggest users to follow based on mutual connections.

3) Post Search
   - Search posts by word or topic.

Command-Line Operations
====================

| Operation                 | Command                                              | Description                                                        |
|----------------------------|------------------------------------------------------|--------------------------------------------------------------------|
| Check Consistency          | XML_Editor verify -i sample.xml                     | Validate XML and report errors.                                     |
| Format XML (Prettify)      | XML_Editor format -i sample.xml -o output.xml      | Format XML with proper indentation.                                 |
| Convert to JSON            | XML_Editor tojson -i sample.xml -o output.json      | Convert XML to JSON format.                                         |
| Minify XML                 | XML_Editor minify -i sample.xml -o output.xml       | Remove unnecessary spaces and newlines.                             |
| Compress XML               | XML_Editor compress -i sample.xml -o output.comp    | Compress the XML file.                                              |
| Decompress XML             | XML_Editor decompress -i sample.comp -o output.xml | Restore original XML from compressed file.                          |
| Most Active Users          | XML_Editor most_active -i sample.xml                | Show the most active users.                                         |
| Most Influential Users     | XML_Editor most_influencer -i sample.xml            | Identify users with the highest influence.                          |
| Post Search by Word        | XML_Editor search -w word -i sample.xml             | Search posts containing a specific word.                            |
| Post Search by Topic       | XML_Editor search -t topic -i sample.xml            | Search posts related to a specific topic.                           |

GUI Features
====================
🗂️ File Input:
   - Load XML via browsing or direct input.

🎛️ Operations Control:
   - Trigger operations via buttons (verify, format, convert, minify, etc.).

📄 Output Display:
   - Show results in read-only text area.
   - Option to save output to a file.

Links
=======
video https://drive.google.com/file/d/12m9jHX5fMgNlRnRFi7Et_qo_AsQPVByD/view?usp=drivesdk

Summary
=======
A compact XML Editor integrating validation, formatting, JSON conversion, minification,
compression, and basic social network analysis using efficient data structures.
