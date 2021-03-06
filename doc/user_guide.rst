
.. _user_guide:

********************
User Guide
********************

How to get bioGUI?
==================

Download bioGUI releases from the `github releases <http://github.com/mjoppich/bioGUI/>`_.
Please note additional information provided in our :ref:`user_guide`.

.. _download_templates:

Install bioGUI
==============

Binary packages/releases for the following operating system are provided on github.

Windows
-------

Extract the zip-File and place the contained bioGUI folder somewhere on your hard drives. Please do not put bioGUI in your Program Files directory, as this is specially protected by Windows and may cause problems.

Linux
-----

Extract the `tar.gz` file and place the contained bioGUI folder somewhere on your hard drives.

Mac OS
------

Download and open the provided dmg package. You can simply drag and drop the bioGUI.app into your Applications folder.
In order to productively use bioGUI, it may be required to install the OSX command line tools as well as brew.
Most (install) templates will require you to have wget and netcat installed. Using brew installation is simple:

.. code:: bash
    
    brew install wget netcat

You can get brew from `here <http://brew.sh/>`_

Downloading Templates
=====================


Part of *bioGUI* is a user-friendly accessibility of templates for various programs.
Therefore, application developers and sophisticated users can upload their templates to our website in order to make their template available to other users.
The user has to submit his name, eMail address, a template name and the template itself.
He can select whether he wants to be an anonymous user (user name is always hidden) and whether this is an installation script (which downloads and creates the \ac{GUI} template specifically for this installed application) or a regular template.
Additionally, categories for the template can be supplied, e.g. whether this is a template for a sequencing tool, or proteomics.
A screenshot of the template submission is shown below.

.. image:: ./images/templates/template_submission.png
   :scale: 50

Additionally the (`bioGUI website <https://www.bio.ifi.lmu.de/software/bioGUI>`_) also contains a searchable list of already available templates.
Available templates are only shown and can be downloaded via the *bioGUI* application.

.. image:: ./images/templates/existing_templates.png
   :scale: 50

Within *bioGUI*, clicking the *Download Templates* button, a new dialog window opens showing a list of available templates.
Columns can be sorted by double clicking the header, and using the search only templates which contain the searched words are shown.
Upon selecting one or multiple (keep ctrl-key down while clicking) rows and clicking the *Download* button, those templates are downloaded and available for the user.
Since it is possible to copy or alter templates, *bioGUI* never overwrites existing templates, but will create a new copy.

.. image:: ./images/templates/biogui_selection.png
   :scale: 50

.. _install_download_templates:

Installing new software with Install Modules
--------------------------------------------

After downloading an *Install Module*, the *Install Template Module* will list the downloaded install template.
The screenshot below shows the *Install Template Module*.
In the dropdown menu, the *hisat2* install template has been chosen.
Since this module is executed on Windows, *WSL* is selected.
*hisat2* is supposed to be installed into the *User Data Dir*, which is on *WSL* and linux usually ``~/.local/share/bioGUI``.
In order to automatically install dependencies, the sudo-password is supplied, which has been set up during installing the *WSL* feature.
Finally the path for the specific *GUI* template has to be set. *bioGUI* by default only searches the template directory besides the executable.
*GUI* templates must have the file extension ``.gui``. If your entered template name does not end with this extension, the extension is appended to your filename.


.. image:: ./images/usage/install_template.PNG
   :scale: 50