#!/usr/bin/perl
##!D:/Perl/bin/perl.exe

#############################################################################
#XMLBoard Solo : An XML-Driven Multi-Forum Message Board v 1.5              #
#Copyright (C) 2002 Joel Rosen                                              #
#WWW: http://xmlboard.hypermart.net                                         #
#Contact: rosenjoel@yahoo.com                                               #
#                                                                           #
#This program is free software; you can redistribute it and/or              #
#modify it under the terms of the GNU General Public License                #
#as published by the Free Software Foundation; either version 2             #
#of the License, or (at your option) any later version.                     #
#                                                                           #
#This program is distributed in the hope that it will be useful,            #
#but WITHOUT ANY WARRANTY; without even the implied warranty of             #
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              #
#GNU General Public License for more details.                               #
#                                                                           #
#You should have received a copy of the GNU General Public License          #
#along with this program; if not, write to the Free Software                #
#Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.#
#############################################################################

# Use the file copying module (for cross-platform use)

use File::Copy;

# File locking:

use Fcntl qw(:DEFAULT :flock);

###### VARIABLE DECLARATION ######
#
### Location-related values:
#
# The absolute location of this script: (I recommend you do not change
# this from the value "$ENV{'SCRIPT_NAME'}", as this value should
# automatically use the correct location.)
 
$boardLocation = $ENV{'SCRIPT_NAME'};

# The absolute location of the document type definition, including the
# filename: (must begin with a /)

$dtdLocation = "/path/to/msg.dtd";

# A list of name and value pairs of the boards and the directory of the boards'
# data, i.e.:
#
# %boards = ("boardName1", "location1", "boardName2", "location2");
#
# Use as many as you like but at least one pair is required.
# Note:  The locations must NOT end with a "/"!
 
%boards = ("forum", "forum");



$MessageTextArea="message2";

#
###
#
###### END OF VARIABLE DECLARATION ######


###### INDEX HANDLERS ######
#
### Start tag handler:
#

sub index_handle_start {
    my ($type, $attName, $attVal) = @_;

  # If it has entered a post element, increase the var:

    if ($type eq "post") {
	$globalWithinElementPost++;
    }

  # If a base id is specified (for a read request):

    if ($baseId) {
	if ($type eq "post") {

  # If the post wanted has been found and the element depth has gone back down
  # to that of the posts, set baseDepth to 0 (false) so that it stops looking:

	    if ($baseDepth && $baseDepth == $globalWithinElementPost + 1) {
		$baseDepth = 0;
	    }

  # If the desired post is found, set the baseDepth to the depth of the next
  # post up (the responses to this post):

	    if ($baseId == $attVal) {
		$baseDepth = $globalWithinElementPost + 1;
	    }
	    else {

  # Get the id for this post if it is one of the responses:

		if ($baseDepth) {
		    $POST{'id'} = $attVal;
		}
	    }
	}
    }

  # If the user requested an index:

    else {
	if ($type eq "board") {
	    if ($useCookies) {          #if cookies are enabled, find the
		$num = $attVal;         #number of posts and initialize the
		&initCookies;           #cookies info
	    }
	    
	    &htmlTop($FORM{'action'});    #print the top of the page for index

	    &showLinks;   #print some links

	    #print "<h1>Message Index</h1>\n";

	    if ($useCookies) {
		&cookieWelcome;    #print a cookie welcome message
	    }

  # If the config is set up for an unordered list, print "<ul>", else
  # print a table for the index:
	    if ($uList) {
		print "<ul>\n";
	    }
	    else {
		print "<table border=\"$border\" cellspacing=\"$cellSpacing\" cellpadding=\"$cellPadding\">\n";
	    }

	}
	elsif ($type eq "post") {

	    $POST{'id'} = $attVal;

  # Set a baseDepth that will always be less than the current depth, so that
  # all of the posts print out:
	    $baseDepth = -1;
	}
    }
}



sub send_admin_note {
    my ($info) = @_;
	$info =~ tr/\"/ /;
	
	system ("echo \"$info\" | mail -s \"forum was updated\" sokirko\@yandex.ru");

}




#sub send_admin_note {
#    my ($info) = @_;
#	print "================  Sending mail =================== ";
#	open(SENDMAIL, "|/usr/sbin/sendmail") or die "Cannot open /usr/sbin/sendmail: $!";
#	print SENDMAIL "Reply-to: asokirko\@list.ru\n";
#	print SENDMAIL "Subject: forum was updated\n";
#	print SENDMAIL "To: asokirko\@list.ru\n";
#	print SENDMAIL "Content-type: text/plain\n\n";
#	print SENDMAIL "Amessage has been posted to the forum of aot.ru: $info\n";
#	close(SENDMAIL);
#}


#
###

### End tag handler:
#

sub index_handle_end {
    my ($type) = @_;
  # If it's a post, decrease the depth var:

    if ($type eq "post") {
	$globalWithinElementPost--;
    }

  # If all the values have been found, print out a link
  # and clear the values.

    my $printLink = 1;
    foreach my $tag (keys %wantedTags) {
	if (!(exists $POST{$tag})) {
	    $printLink = 0;
	}
    }

  # If printLink is still true, make sure the post is recent enough to be
  # printed in the index:

    if ($printLink) {
	my $now = time;
	if ($now - $POST{'date'} > $recent && $recent) {
	    $printLink = 0;

  # Record that a post has been skipped at this depth in the index:

	    $skippedAtDepth{$globalWithinElementPost} = 1;
  
  # Clear the values (if the post is expired):
	    
	    foreach my $key (keys %POST) {
		delete $POST{$key};
	    }

	}
    }

    if ($printLink) {

  # Verify that posts have been found and printed. (if no posts are found,
  # and the user requested the "read" action, it will write a message 
  # saying that there are no responses.)

	$existResponses = 1;

	my $niceDate = &niceTime($POST{'date'}); # format the date nicely
	
  # Table formatting if $uList != 1

	if (!$uList) {
	    print "<tr>\n";
	}


  # Instead of using a list, just indent depending on the depth of the current
  # post element. (subtract 1 to not indent the base posts)

	my $depth = $globalWithinElementPost - 1;
	my $difference = 0;

  # Indent less depending on the number of parent posts for this post that are
  # expired.

	foreach my $k (keys %skippedAtDepth) {
	    $difference++;
	}

  # Indent less if it is for a read request (should show the immediate 
  # responses to the post requested as not indented.)

	if ($baseDepth > 1) {
	    $difference += $baseDepth - 1;
	}

	$depth -= $difference;

  # Print the appropriate number of <ul>s or </ul>s if $uList = 1:

	if ($uList) {
	    if ($depth < $ulDepth) {
		for (my $i = 0; $i < $ulDepth - $depth; $i++) {
		    print "</ul>\n";
		}
		$ulDepth = $depth;
	    }
	    elsif ($depth > $ulDepth) {
		print "<ul>\n";
		$ulDepth++;
	    }

  # Print the <li> tag with the specified type:

	    if ($liType =~ m/^(circle|square|disc)$/i) {
		print "<li type=\"$liType\">";
	    }
	    else {
		print "<li>";
	    }
	}

  # Otherwise, print the appropriate number of spaces:

	else {
	    if ($cellColor) {
		print "<td bgcolor=\"$cellColor\" NOWRAP>\n";
	    }
	    else {
		print "<td NOWRAP>\n";
	    }

	    for (my $i = 0; $i < $depth; $i++) {
		for (my $j = 0; $j < $indentDepth; $j++) {
		    print "&nbsp;";
		}
	    }
	    print "<b>$bulletChar</b>\n";
	}

  # If the user wants to delete posts, put radio buttons next to the post
  # for selection:

	if ($admin) {
	    print "<input type=\"radio\" name=\"rmId\" value=\"$POST{'id'}\">\n";
	}


  # If cookies are on and the post is new, make an indication:

	if ($useCookies == 1) {
	    if ($POST{'date'} > $COOKIES{'VisitTemp'}) {
		print "<b><i><font color=\"$newColor\">new</font></i></b> ";
	    }
	}

  # Format a link:

	print "<a href=\"$boardLocation?board=$FORM{'board'}&action=read&id=$POST{'id'}\">$POST{'subject'}</a> - $POST{'name'} - $niceDate\n";

  # If $uList != 1, print table formatting:

	if (!$uList) {
	    print "</td></tr>\n";
	}

  # Clear the values:

	foreach my $key (keys %POST) {
	    delete $POST{$key};
	}

    }
    

    if ($type eq "board") {

  # If configured for an unordered list, close all lists:

	if ($uList) {
	    for (my $i = 0; $i <= $ulDepth; $i++) {
		print "</ul>\n";
	    }
	}
	else {
	    print "</table>\n";
	}

    }
    elsif ($type eq "post") {

  # If it encounters a close post tag, and a post at this depth has been
  # skipped because it was not recent enough, remove the depth number 
  # as one that has been skipped. This way, it won't look like posts in other
  # threads are in response to old posts. (they won't be indented less)

	my $postDepth = $globalWithinElementPost + 1;
	if (exists $skippedAtDepth{$postDepth}) {
	    delete $skippedAtDepth{$postDepth};
	}
    }

}

#
###

### Character data handler:
#

sub index_handle_char {
    my ($data) = @_;

  # The handle_char sub is called several times for one
  # piece of data, if the data spans multiple lines,
  # so the values must be concatenated to form entire values, or
  # sentences, or whatever. Also, this will only get data if the post
  # is within the specified depth.

    my $currentElement = $globalInElement[$globalDepth];
    if (exists $wantedTags{$currentElement} && $baseDepth && $baseDepth < $globalWithinElementPost + 1) {
	$POST{$currentElement} .= $data; 
    }

}

#
###
#
###### END OF INDEX HANDLERS ######

###### READ HANDLERS ######
#
### Start tag handler:
#

sub read_handle_start {
    my ($type, $attName, $attVal) = @_;

  #If it's a post tag, increase the depth var:

    if ($type eq "post") {
	$globalWithinElementPost++;
    }

    # If this is parsing for the requested post:
    
    if ($type eq 'post') {
	if (! $secondCheck) {
	    my $depth = $globalWithinElementPost - 1;
	    if ($FORM{'id'} == $attVal) {
		$foundPost = 1;
		$POST{'id'} = $attVal;
		if ($depth == 0) {        # If it is not a followup, do not
		    $checkTwice = 0;      # parse the document twice.
		}                         # Otherwise, set $FORM{'id'} to the
		else {                    # id of the mother post.
		    $checkTwice = 1; 
		    $FORM{'id'} = $idList[$depth - 1];
		}
	    }
	    else {
		$foundPost = 0;
		# Make a list of possible previous posts' ids:
		$idList[$depth] = $attVal;
	    }
	}
	
	# If it has discovered the previous post's id and is looking for
	# the previous post's info. (second parse)
	
	else {
	    if ($FORM{'id'} == $attVal) {
		$foundPost = 1;
		$PREVPOST{'id'} = $attVal;
	    }
	    else {
		$foundPost = 0;
	    }
	}   
    }
    elsif ($type eq 'board') {
	if ($useCookies) {        #if cookies are enabled, get the number
	    $num = $attVal;       #of posts and initialize them
	    &initCookies;
	}                      
    }
    
    # In case the elements are found, but empty, this will ensure that 
    # the check for all tags will pass, by creating them in the hash:
    
    if ($foundPost) {
	if ($secondCheck && (exists $prevWantedTags{$type})) {
	    $PREVPOST{$type} = '';
	}
	elsif ((! $secondCheck) && (exists $wantedTags{$type})) { 
	    $POST{$type} = '';
	}
    }
}

#
###

### End tag handler:
#

sub read_handle_end {
	
    my ($type) = @_;



  # If it's a post, decrease the depth var:

    if ($type eq "post") {
	$globalWithinElementPost--;
    }

  # Check to see if all the needed info has been found for the post:

    my $printPost = 1;
    foreach my $tag (keys %wantedTags) 
	{
		if (!(exists $POST{$tag})) 
		{
	    	$printPost = 0;
		}
    }

    if ($printPost) {
	$postExists = 1;    # Set a variable verifying existence of the post.
    }

  # Check to see if a second post is needed, and if it is, check to see
  # if the info for that post has been found.

    if ($checkTwice) {
	foreach my $tag (keys %prevWantedTags) {
	    if (!(exists $PREVPOST{$tag})) {
		$printPost = 0;
	    }
	}
    }

  # If printPost is still true (the previous two tests have passed),
  # the page will print out.

    if ($printPost) {
	&htmlTop($FORM{'action'}); # Print the top of the page
	$foundPost = 0;

  # If the ip address should be hidden, print it as a comment:
	if (! $showIp) {
	    print "<!-- IP Logged: $POST{'ip'} -->\n";
	}

	&showLinks;   #print some links

	print "<h1>$POST{'subject'}</h1>\n";

  # If the user included an email, print their name as a "mailto:" link:

        if ($POST{'email'}) {
	    print "Posted by <a href=\"mailto:$POST{'email'}\">$POST{'name'}</a>\n";
	}
	else {
	    print "Posted by $POST{'name'}\n";
	}

  # If the ip address should be shown, print it next to the name:

	if ($showIp) {
	    print "<i>($POST{'ip'})</i>\n";
	}

        my $niceDate = &niceTime($POST{'date'});

  # If this post is a reply, print info about the post it is in response to:

        if ($checkTwice) {
	    my $prevNiceDate = &niceTime($PREVPOST{'date'});
            print "on $niceDate, in response to <a href=\"$boardLocation?board=$FORM{'board'}&action=read&id=$PREVPOST{'id'}\">$PREVPOST{'subject'}</a>, posted on $prevNiceDate by ";

  # If the author of the previous post gave their email address, print their
  # name as a link:

	    if ($PREVPOST{'email'}) {
		print "<a href=\"mailto:$PREVPOST{'email'}\">$PREVPOST{'name'}</a>.\n";
	    }
	    else {
		print "$PREVPOST{'name'}.\n";
	    }
	}
	else {
	    print "on $niceDate.\n";
	}

  # Replace \n with <br> and print the message:

	$POST{'message'} =~ s/\n/<br>/g;

	print "<p>$POST{'message'}</p>\n";

  # Format and print a link, if given:

	print "<h1>Responses:</h1>\n";

  # Set baseId for the index sub:
	$baseId = $POST{'id'};

  # Remember what the subject is for a response input:
	$postSubject = $POST{'subject'};

  # Remember what the message is for a response textarea:
	$postMessage = $POST{'message' };

  # Clear everything so the the index parse can run:
	foreach my $key (keys %POST) {
	    delete $POST{$key};
	}
	foreach $key (keys %PREVPOST) {
	    delete $PREVPOST{$key}
	}
       	foreach $key (keys %wantedTags) {
	    delete $wantedTags{$key}
	}
	$stopParse = 1;
    }  
}

#
###

### Character data handler:
#

sub read_handle_char {
    my ($data) = @_;

    my $currentElement = $globalInElement[$globalDepth];

  # If this is the first parse:

    if (! $secondCheck) {
	if ($foundPost) {
	    if (exists $wantedTags{$currentElement}) {
		$POST{$currentElement} .= $data;
	    }
	}
    }

  # If this is the second parse:

    else {
	if ($foundPost) {
	    if (exists $prevWantedTags{$currentElement}) {
		$PREVPOST{$currentElement} .= $data;
	    }
	}
    }
}

#
###
#
###### END OF READ HANDLERS ######

###### WRITE HANDLERS ######
#
### Start tag handler:
#

sub write_handle_start {
    my ($type, $attName, $attVal) = @_;


  # If it's a post, increment the depth var:

    if ($type eq "post") {
	$globalWithinElementPost++;
    }

  # Put indentation for nicely formatted XML:
    &indent;


  # If the tag is <board>, get the number of posts, open the xml file for
  # writing, and update the number.

    if ($type eq "board") {

  # Add the xml version declaration and the doctype declaration:

	print MSGOUT "<?xml version=\"1.0\"?>\n";
	print MSGOUT "<!DOCTYPE board SYSTEM \"$dtdLocation\">\n";

	$num = $attVal + 1;
	print MSGOUT "<board num=\"$num\">\n";

  # If the post is not a response, just write the values immediately beneath
  # the <board> tag:

	if (! $FORM{'prevId'}) {
	    &writePost;
	}
    }

  # If the tag is <post>, check to see if it is the post we want to reply to,
  # if the post is a reply.

    elsif ($type eq "post") {
	if ($FORM{'prevId'} && $attVal == $FORM{'prevId'}) {
	    $foundPost = 1;
	}
	print MSGOUT "<post id=\"$attVal\">\n";
    }

  # If it's any other tag, just spit it back out:

    else {
	print MSGOUT "<$type>";
	if (exists $cdataTags{$type}) {
	    print MSGOUT "<![CDATA[";
	}
    }

  # If this is the post we want, add the type of tag found to the POST hash:

    if ($foundPost == 1 && (exists $wantedTags{$type})) {
	$POST{$type} = '';
    }
    
}

#
###

### End tag handler:
#

sub write_handle_end {
    my ($type) = @_;

  # If it's a post, decrease the depth var:

    if ($type eq "post") {
	$globalWithinElementPost--;
    }

  # Indent the tag if it's a post tag:
    if ($type eq "post") {
	&indent;
    }
    if (exists $cdataTags{$type}) {
	print MSGOUT "]]>";
    }
    print MSGOUT "</$type>\n";
    if ($foundPost) {
	my $printPost = 1;
	foreach my $tag (keys %wantedTags) {
	    if (!(exists $POST{$tag})) {
		$printPost = 0;
	    }
	}

	if ($printPost) {
	    &writePost;
	    $foundPost = 0;
	}
    }

  # If the tag is a close board tag,
  # stop parsing:

    if ($type eq "board") {

	if ($useCookies) {
	    my $expires = localtime(time + $expiresIn);
	    &setCookie(0, "LastVisit", $COOKIES{'LastVisit'}, "LastNum", $COOKIES{'LastNum'}, "Name", $FORM{'name'}, "email", $FORM{'email'});

	}
	$stopParse = 1;
    }


}

#
###

### Character data handler:
#

sub write_handle_char {
    my ($data) = @_;
    my $currentElement = $globalInElement[$globalDepth];
    if (exists $wantedTags{$currentElement}) {
	print MSGOUT "$data";
    }
}

#
###
#
###### END OF WRITE HANDLERS ######

###### REMOVE HANDLERS ######
#
### Start tag handler:
#

sub remove_handle_start {
    my ($type, $attName, $attVal) = @_;

  # If it's a post, increase the depth var:

    if ($type eq "post") {
	$globalWithinElementPost++;
    }

  # If it has found the <board> tag, print out the XML headings, etc.
    if ($type eq "board") {
	print MSGOUT "<?xml version=\"1.0\"?>\n";
	print MSGOUT "<!DOCTYPE board SYSTEM \"$dtdLocation\">\n";
    }

  # If the thread specified for removal has not been found...

    if (! $foundPost) {

  # If the tag is a post tag and it is the post specified for removal...

	if ($type eq "post" && $attVal == $FORM{'rmId'}) {

  # Set a variable that the post has been found:

	    $foundPost = 1;

  # Remember the depth at which the post was found:

	    $baseDepth = $globalWithinElementPost - 1;
	}

  # Otherwise, indent and print the tag:

	else {
	    &indent;
	    if ($attName) {
		print MSGOUT "<$type $attName=\"$attVal\">";
	    }
	    else {
		print MSGOUT "<$type>";
	    }
	    if ($type eq "post" || $type eq "board") {
		print MSGOUT "\n";
	    }
	    if (exists $cdataTags{$type}) {
		print MSGOUT "<![CDATA[";
	    }

	}
    }
}
#
###

### End tag handler:
#

sub remove_handle_end {
    my ($type) = @_;


  # If it's a post, decrease the depth var:

    if ($type eq "post") {
	$globalWithinElementPost--;
    }

  # If the post specified has not yet been found...

    if (! $foundPost) {

  # Indent the tag if it's a post tag:

	if ($type eq "post") {
	    &indent;
	}
	if (exists $cdataTags{$type}) {
	    print MSGOUT "]]>";
	}
	print MSGOUT "</$type>\n";
	
    }

  # If the post has been found and the parser is in that post's thread...

    else {

  # If the parser has arrived at the depth at which the post was found,
  # set foundPost = 0.

	if ($globalWithinElementPost == $baseDepth) {
	    $foundPost = 0;
	}
    }

}

#
###

### Character data handler:
#

sub remove_handle_char {
    my ($data) = @_;

  # If parser is not within the thread of the post specified for removal, 
  # print the data.

    if (! $foundPost) {
	my $currentElement = $globalInElement[$globalDepth];
	if (exists $wantedTags{$currentElement}) {
	    print MSGOUT "$data";
	}
    }
}

#
###
#
###### END OF REMOVE HANDLERS ######

###### REMOVE OLD HANDLERS ######
#
### Start tag handler:
#

sub removeold_handle_start {
    my ($type, $attName, $attVal) = @_;

  #If it's a post tag, increase the depth var:

    if ($type eq "post") {
	$globalWithinElementPost++;
    }

  # If it has found the <board> tag, print out the XML headings, etc.
    if ($type eq "board") {
	print MSGOUT "<?xml version=\"1.0\"?>\n";
	print MSGOUT "<!DOCTYPE board SYSTEM \"$dtdLocation\">\n";
	print MSGOUT "<board $attName=\"$attVal\">\n";
    }

  # If it is a <post> tag, get the id number:

    elsif ($type eq "post") {
	$POST{'id'} = $attVal;
    }

  # Note that the element type has been found, even if empty:

    if (exists $wantedTags{$type}) { 
	$POST{$type} = '';
    }

}
#
###

### End tag handler:
#

sub removeold_handle_end {
    my ($type) = @_;

  # If it's a post, decrease the depth var:

    if ($type eq "post") {
	$globalWithinElementPost--;
    }

  # Check to see if all tags for a post have been found:

    my $printPost = 1;
    foreach my $tag (keys %wantedTags) {
	if (!(exists $POST{$tag})) {
	    $printPost = 0;
	}
    }

  # Check to see if the post is too old to be printed (and thus deleted):

    if ($printPost) {
	my $now = time;
	if ($POST{'date'} <= $now - $FORM{'rmAge'}) {
	    $printPost = 0;

  # Record that a post has been skipped at this depth in the parse:

	    $skippedAtDepth{$globalWithinElementPost} = 1;
  
  # Clear the values (if the post is expired):
	    
	    foreach my $key (keys %POST) {
		delete $POST{$key};
	    }
	}
    }

  # Print the post if the above tests have passed:

    if ($printPost) {

  # Slightly modified version of the indent subroutine for the first <post>
  # tag:

	my $indentDepth = $globalDepth - 1;
	my $difference = 0;
	foreach my $k (keys %skippedAtDepth) {
	    $difference++;
	}
	$indentDepth -= $difference;
	for (my $i = 0; $i < $indentDepth; $i++) {
	    print MSGOUT '  ';
	}

  # Print the whole post back out, with values stored in %POST:

	print MSGOUT "<post id=\"$POST{'id'}\">\n";  &indent;
	print MSGOUT "<name><![CDATA[$POST{'name'}]]></name>\n";  &indent;
	print MSGOUT "<date>$POST{'date'}</date>\n";  &indent;
	print MSGOUT "<email><![CDATA[$POST{'email'}]]></email>\n";  &indent;
	print MSGOUT "<ip>$POST{'ip'}</ip>\n";  &indent;
	print MSGOUT "<subject><![CDATA[$POST{'subject'}]]></subject>\n";  &indent;
	print MSGOUT "<message><![CDATA[$POST{'message'}]]></message>\n";  &indent;
	print MSGOUT "<link><![CDATA[$POST{'link'}]]></link>\n";

  # Clear the values:
	    
	foreach my $key (keys %POST) {
	    delete $POST{$key};
	}
    }

  # If it is a </post> tag, check to see if a post has been skipped at this
  # depth because it was too old, otherwise print out </post>:

    if ($type eq "post") {
	my $postDepth = $globalWithinElementPost + 1;
	if (exists $skippedAtDepth{$postDepth}) {
	    delete $skippedAtDepth{$postDepth};
	}
	else {
	    &indent;
	    print MSGOUT "</post>\n";
	}
    }
    elsif ($type eq "board") {
	print MSGOUT "</board>\n";
	$stopParse = 1;
    }
}

#
###

### Character data handler:
#

sub removeold_handle_char {
    my ($data) = @_;

    my $currentElement = $globalInElement[$globalDepth];
    if (exists $wantedTags{$currentElement}) {
	$POST{$currentElement} .= $data; 
    }
}

#
###
#
###### END OF REMOVE OLD HANDLERS ######

###### SEARCH HANDLERS ######
#
### Start tag handler:
#

sub search_handle_start {
    my ($type, $attName, $attVal) = @_;

  # If it's a post, get the id number:

    if ($type eq "post") {
	$POST{'id'} = $attVal;
    }

    if (exists $wantedTags{$type}) { 
	$POST{$type} = '';
    }
}

#
###

### End tag handler:
#

sub search_handle_end {
    my ($type) = @_;

  # Check to see if all the post info has been gathered:

    my $searchData = 1;
    my $printLink = 0;
    foreach my $tag (keys %wantedTags) {
	if (!(exists $POST{$tag})) {
	    $searchData = 0;
	}
    }

  # If the post info has been gathered, concatenate all the requested info 
  # and run a regexp:

    my $searchText = '';

    if ($searchData) {

  # If looking for all words, assume it will print and then set printLink to
  # 0 when it does not find a word.  Otherwise, it will set printLink to 1
  # when it does find a word.

	if ($FORM{'matchType'} eq "all") {
	    $printLink = 1;
	}
	else {
	    $printLink = 0;
	}

	if ($FORM{'checkName'}) {
	    $searchText .= ' ' . $POST{'name'};
	}
	if ($FORM{'checkSubject'}) {
	    $searchText .= ' ' . $POST{'subject'};
	}
	if ($FORM{'checkMessage'}) {
	    $searchText .= ' ' . $POST{'message'};
	}

  # If case sensitive is specified:

	if ($FORM{'caseSensitive'}) {
	    foreach my $searchKey (@searchKeys) {
		if ($searchText =~ m/^(.*?[^A-Za-z])?$searchKey([^A-Za-z].*)?$/s) {
		    if ($FORM{'matchType'} ne "all") {
			$printLink = 1;
		    }
		}
		else {
		    if ($FORM{'matchType'} eq "all") {
			$printLink = 0;
		    }
		}
	    }
	}

  # Otherwise ignore case:

	else {
	    foreach my $searchKey (@searchKeys) {
		if ($searchText =~ m/^(.*?[^A-Za-z])?$searchKey([^A-Za-z].*)?$/si) {
		    if ($FORM{'matchType'} ne "all") {
			$printLink = 1;
		    }
		}
		else {
		    if ($FORM{'matchType'} eq "all") {
			$printLink = 0;
		    }
		}
	    }
	}
    

  # If the pattern matched, print a link:

	if ($printLink) {

  # Note that at least one match was made:

	    $searchPassed = 1;
	    $printLink = 0;
	    
	    if ($uList) {

  # Print the <li> tag with the specified type:
		
		if ($liType =~ m/^(circle|square|disc)$/i) {
		    print "<li type=\"$liType\">";
		}
		else {
		    print "<li>";
		}
	    }
	    
  # Otherwise, print the table formatting:
	    
	    else {
		
		if ($cellColor) {
		    print "<td bgcolor=\"$cellColor\" NOWRAP>\n";
		}
		else {
		    print "<td NOWRAP>\n";
		}
		
		print "<b>$bulletChar</b>\n";
	    }
	    
  # Format a link:
	    
	    my $niceDate = &niceTime($POST{'date'}); # format the date nicely
	    
	    print "<a href=\"$boardLocation?board=$FORM{'board'}&action=read&id=$POST{'id'}\">$POST{'subject'}</a> - $POST{'name'} - $niceDate\n";
	    
  # If $uList != 1, print table formatting:
	    
	    if (!$uList) {
		print "</td></tr>\n";
	    }
	}
  # Clear the values:

	foreach my $key (keys %POST) {
	    delete $POST{$key};
	}
    }
}

#
###

### Character data handler:
#

sub search_handle_char {
    my ($data) = @_;

    my $currentElement = $globalInElement[$globalDepth];
    if (exists $wantedTags{$currentElement}) {
	$POST{$currentElement} .= $data;
    }

}

#
###
#
###### END OF SEARCH HANDLERS ######


### Top of the page html sub:
#

sub htmlTop {
    my ($action) = @_;
    print "Content-type:text/html\n\n";
    print "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Final//EN\">\n";

	print "<html>";
	print "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\">";
    print "<head><title>$titlePrefix\n";

    if ($action eq "index") {
	if ($admin) {
	    print "Administration\n";
	}
	else {
	    print "Message Index\n";
	}
    }
    elsif ($action eq "read") {
	print "$POST{'subject'}\n";
    }
    elsif ($action eq "search") {
	print "Search Results\n";
    }
    print "</title>\n";

  # Include the html from top.html, processing the lines for including other  
  # HTML files. 

    open (TOP, "$boards{$FORM{'board'}}/top.html");
    while (my $line = <TOP>) {
	if ($line =~ /^\s*<\?\s*xmlboard\s+include\s*=\s*\"(.*)\"\s*\?>\s*$/) {
	    open (INCLUDE, "$1");
	    while (my $includeLine = <INCLUDE>) {
		print "$includeLine";
	    }
	    close (INCLUDE);
	}
	else {
	    print "$line";
	}
    }
    close (TOP);



  # If the user is deleting posts, add html for a form:

    if ($admin) {
	print "<form method=\"post\" action=\"$boardLocation\">\n";
    }

}

#
###

### Bottom of the page html sub:
#

sub htmlBottom {
    open (BOTTOM, "$boards{$FORM{'board'}}/bottom.html");
    while (my $line = <BOTTOM>) {
	if ($line =~ /^\s*<\?\s*xmlboard\s+include\s*=\s*\"(.*)\"\s*\?>\s*$/) {
	    open (INCLUDE, "$1");
	    while (my $includeLine = <INCLUDE>) {
		print "$includeLine";
	    }
	    close (INCLUDE);
	}
	else {
	    print "$line";
	}
    }
    close (BOTTOM);
    print "<!-- Generated by Joel Rosen's XMLBoard (C) 2000 -->\n";
}

#
###

### Get the GET or POST values into the %FORM hash:
#

sub getForm {

  # Check the method (GET or POST):

    if ($ENV{'REQUEST_METHOD'} eq 'POST') {
	read (STDIN, $buffer, $ENV{'CONTENT_LENGTH'});
    }
    else {
	$buffer = $ENV{'QUERY_STRING'};
    }

  # Decipher the data, and de-html-ize stuff, putting it into %FORM:

    @pairs = split(/&/, $buffer);
    foreach $pair (@pairs) {
	($name, $value) = split (/=/, $pair);
	$value =~ tr/+/ /;
	$value =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg;
	$value =~ s/&/&amp\;/g;
	$value =~ s/</&lt\;/g;
	$value =~ s/>/&gt\;/g;

  # Turn extended ASCII characters into HTML equivalents:

	
#for (my $chrNum = 161; $chrNum < 256; $chrNum++) {
#	    my $asciiChar = chr($chrNum);
#	    $value =~ s/$asciiChar/\&\#$chrNum\;/g;
#	}

# Make sure there are no non-US characters left:
#	$value =~ s/[^\w\s\-\_\=\+\~\!\@\#\$\%\^\&\*\(\)\\\/\`\;\:\'\"\,\.\?\[\]\{\}\>\<]//g;

	$FORM{$name}=$value;
    }
  
}

#
###

### Format the time nicely:
#

sub niceTime {
    my($InTime) = @_;
    my ($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = localtime($InTime);
    my @Months = ("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec");
    my @Days = ("Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat");
    $year = 1900 + $year;
    $mon = $Months[$mon];
    $wday = $Days[$wday];

    #change any single digit minutes into two digits with a zero in the front

    $min =~ s/^(\d)$/0$1/;

    my $AmPm;
    if ($hour >= 0 && $hour < 12) {
	$AmPm = 'AM';
    }
    else {
	$AmPm = 'PM';
    }

    if ($hour > 12) {
	$hour -= 12;
    }
    elsif ($hour == 0) {
	$hour = 12;
    }

    $OutTime = "$wday, $mon $mday, $year, $hour:$min $AmPm";

    return $OutTime;
}

#
###

### Define a hash of the tags wanted for the action requested:
#

sub setTags {
    my ($action) = @_;
    if ($action eq "index") {
	%wantedTags = (name => '',
		       date => '',
		       subject => '');
    }
    elsif ($action eq "read") {
	%wantedTags = (name => '',
		       date => '',
		       ip => '',
		       email => '',
		       subject => '',
		       message => '',
		       link => '');
	%prevWantedTags = (name => '',
			   date => '',
			   subject => '',
			   email => '');
    }
    elsif ($action eq "search") {
	%wantedTags = (name => '',
		       date => '',
		       subject => '',
		       message => '');
    }
    elsif ($action eq "write" || $action eq "remove" || $action eq "removeold") {
	%wantedTags = (name => '',
		       date => '',
		       ip => '',
		       email => '',
		       subject => '',
		       message => '',
		       link => '');
	%cdataTags = (name => '',
		      email => '',
		      subject => '',
		      message => '',
		      link => '');
    }

}

#
###

### Prints out the html for a form for posting:
#

sub printForm {
    my ($action) = @_;
    if (! $admin) {
	print "<table><tr><td><h1>\n";
	
	if ($action eq "index") {
	    print "Post a New Message\n";
	}
	elsif ($action eq "read") {
	    print "Post a Reply\n";
	}
	print "</h1></td></tr>\n";
	print "<form method=\"post\" action=\"$boardLocation\">\n";

	print "<tr><td>Name (только латиница)</td></tr>\n";
	print "<tr><td><input type=\"text\" name=\"name\" size=\"$fieldLength\" maxlength=\"$maxLength\" value=\"$COOKIES{'Name'}\"></td></tr>\n";

	print "<tr><td>Сколько будет дважды два? (тест на спамеров)</td></tr>\n";
	print "<tr><td><input type=\"text\" name=\"antispam-test\" size=\"$fieldLength\" maxlength=\"$maxLength\" value=\"\"></td></tr>\n";

	print "<tr><td>Subject</td></tr>\n";
	print "<tr><td>\n";
	if ($postSubject) {

  # Decide whether or not to add Re: to the beginning of the subject:

	    if ($reAlways) {
		$postSubject = 'Re: ' . $postSubject;
	    }
	    else {
		if ($postSubject !~ m/^Re:/) {
		    $postSubject = 'Re: ' . $postSubject;
		}
	    }

	    print "<input type=\"text\" name=\"subject\" size =\"$fieldLength\" maxlength=\"$maxLength\" value=\"$postSubject\">\n";
	}
	else {
	    print "<input type=\"text\" name=\"subject\" size=\"$fieldLength\" maxlength=\"$maxLength\">\n";
	}
	print "</td></tr>\n";
	print "<tr><td>Email\n";
	if (! $needEmail) {
	    print "(optional)\n";
	}
	print "</td></tr>\n";
	print "<tr><td><input type=\"text\" name=\"email\" size=\"$fieldLength\" maxlength=\"$maxLength\" value=\"$COOKIES{'email'}\"></td></tr>\n";
	print "<tr><td>Message</td></tr>\n";
	print "<tr><td><textarea name=\"$MessageTextArea\" rows=\"4\" cols=\"$fieldLength\" ";
	if ($hardWrap) {
	    print "wrap=\"hard\">";
	}
	else {
	    print "wrap=\"virtual\">";
	}
	my @messageLines = split(/<br>/, $postMessage);
	foreach my $line (@messageLines) {
	    print ">$line\n";
	}
	print "</textarea></td></tr>\n";
	if ($action eq "read") {
	    print "<input type=\"hidden\" name=\"prevId\" value=\"$requestedId\">\n";
	}
	print "<input type=\"hidden\" name=\"board\" value=\"$FORM{'board'}\">\n";
	print "<input type=\"hidden\" name=\"action\" value=\"write\">\n";
	print "<tr><td><input type=\"submit\" value=\"Submit\"></td></tr>\n";
	print "</form></table>\n";
    }
    else {

  # If admin = 1, print a remove post form:

	print "<table><tr><td>\n";
	print "<h1>Remove Posts</h1>\n";
	print "</td></tr>\n";
	print "<tr><td>Administrator Name:</td></tr>\n";
	print "<tr><td><input type=\"text\" name=\"adminName\" size=\"$fieldLength\"></td></tr>\n";
	print "<tr><td>Password:</td></tr>\n";
	print "<tr><td><input type=\"password\" name=\"adminPass\" size=\"$fieldLength\"></td></tr>\n";
	print "<input type=\"hidden\" name=\"board\" value=\"$FORM{'board'}\">\n";
	print "<input type=\"hidden\" name=\"action\" value=\"remove\">\n";
	print "<tr><td><input type=\"submit\" value=\"Submit\"></td></tr>\n";
	print "</form></table>\n";


  # Print an additional form for clearing old posts:

	print "<form method=\"post\" action=\"$boardLocation\">\n";
	print "<table><tr><td>\n";
	print "<h1>Remove Old Posts</h1>\n";
	print "</td></tr>\n";
	print "<tr><td>Maximum Age of Posts (in seconds):</td></tr>\n";
	print "<tr><td><input type=\"text\" name=\"rmAge\" size=\"$fieldLength\" value=\"$recent\"></td></tr>\n";
	print "<tr><td>Administrator Name:</td></tr>\n";
	print "<tr><td><input type=\"text\" name=\"adminName\" size=\"$fieldLength\"></td></tr>\n";
	print "<tr><td>Password:</td></tr>\n";
	print "<tr><td><input type=\"password\" name=\"adminPass\" size=\"$fieldLength\"></td></tr>\n";
	print "<input type=\"hidden\" name=\"board\" value=\"$FORM{'board'}\">\n";
	print "<input type=\"hidden\" name=\"action\" value=\"removeold\">\n";
	print "<tr><td><input type=\"submit\" value=\"Submit\"></td></tr>\n";
	print "</form></table>\n";


  # Print an additional form for password changing:
	print "<form method=\"post\" action=\"$boardLocation\">\n";
	print "<table><tr><td>\n";
	print "<h1>Change Name/Password</h1>\n";
	print "</td></tr>\n";
	print "<tr><td>Old Administrator Name:</td></tr>\n";
	print "<tr><td><input type=\"text\" name=\"adminName\" size=\"$fieldLength\"></td></tr>\n";
	print "<tr><td>Old Password:</td></tr>\n";
	print "<tr><td><input type=\"password\" name=\"adminPass\" size=\"$fieldLength\"></td></tr>\n";
	print "<tr><td>New Administrator Name:</td></tr>\n";
	print "<tr><td><input type=\"text\" name=\"newAdminName\" size=\"$fieldLength\"></td></tr>\n";
	print "<tr><td>New Password:</td></tr>\n";
	print "<tr><td><input type=\"password\" name=\"newAdminPass\" size=\"$fieldLength\"></td></tr>\n";
	print "<tr><td>Re-Type Password:</td></tr>\n";
	print "<tr><td><input type=\"password\" name=\"newAdminPassCheck\" size=\"$fieldLength\"></td></tr>\n";
	print "<input type=\"hidden\" name=\"board\" value=\"$FORM{'board'}\">\n";
	print "<input type=\"hidden\" name=\"action\" value=\"changepw\">\n";
	print "<tr><td><input type=\"submit\" value=\"Submit\"></td></tr>\n";
	print "</form></table>\n";
    }
}

#
###

### Prints a page with the specified message:
#

sub message {
    my($msgTitle, $msg) = @_;
    print "Content-type:text/html\n\n";
    print <<EndOfHTML;
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Final//EN">
<html>
<head>
<title>$msgTitle</title>
EndOfHTML

    open (TOP, "$boards{$FORM{'board'}}/top.html");
    while (my $line = <TOP>) {
	if ($line =~ /^\s*<\?\s*xmlboard\s+include\s*=\s*\"(.*)\"\s*\?>\s*$/) {
	    open (INCLUDE, "$1");
	    while (my $includeLine = <INCLUDE>) {
		print "$includeLine";
	    }
	    close (INCLUDE);
	}
	else {	
	    print "$line";
	}
    }
    close (TOP);
    print "<p>\n";
    print "$msg\n";
    print "</p>\n";
    &htmlBottom;
}

#
###

### Makes sure all the needed data is present, print an error message and 
#   quit if not:

sub checkData {

    my %requiredInfo;

  # For the write action:
  
    if ($FORM{'action'} eq "write") {

  # Creates a hash of needed values:

	%requiredInfo = (name => 'Name',
			 subject => 'Subject',
			 $MessageTextArea => 'Message');

  # If the email address is required, add it to the hash:

	if ($needEmail) {
	    $requiredInfo{'email'} = 'Email';
	}

  # If the email address is not a valid email address, get rid of it:

	if ($FORM{'email'} !~ m/^(\w+)(\.(\w+))*@(\w+)(\.(\w+))+$/) {
	    $FORM{'email'} = '';
	}

  # If configured for a language filter, filter away!

	if ($languageFilter) {
	    foreach my $badKey (keys %requiredInfo) {
		$FORM{$badKey} = &playNice($FORM{$badKey});
	    }
	}

    }

  # For the remove action:

    elsif ($FORM{'action'} eq "remove") {

	%requiredInfo = (rmId => 'Post Id');

  # Make sure the id is just a number:

	$FORM{'rmId'} =~ s/[^0-9]//g;
    }

    elsif ($FORM{'action'} eq "removeold") {

	%requiredInfo = (rmAge => 'Post Age');

  # Make sure the age is just a number:

	$FORM{'rmAge'} =~ s/[^0-9]//g;
    }

    my $errorString;
    foreach my $tag (keys %requiredInfo) 
	{
		$FORM{$tag} = &trim($FORM{$tag});
		if (not $FORM{$tag}) 
		{
		    $errorString .= "<li>$requiredInfo{$tag}</li>";
		}
    }

	my $ast =  &trim($FORM{'antispam-test'});
	if ($FORM{'action'} eq "write")
	{
		if (			($ast ne "4") 
				&&      ($ast ne "четыре")
				&&      ($ast ne "Четыре")
				&&      ($ast ne "ЧЕТЫРЕ")
			)
		{
			if ($ast eq "")
			{
				&message("Error", "<h3>Error</h3><p>Нужно ответить на антиспамерский вопрос!!!! <p><a href=\"$ENV{'HTTP_REFERER'}\">Back</a> <a href=\"$boardLocation?board=$FORM{'board'}&action=index\">Message Index</a>");
			}
			else
			{
				&message("Error", "<h2>Access denied!</h2><h3>Наверно, вы спамер, если не знаете, сколько будет дважды два</h3> ");
			}	
			exit;
		}
	}



    if ($errorString) 
	{
		&message("Error", "<h3>Error</h3><p>The following fields must be filled in:</p><ul>$errorString</ul><p><a href=\"$ENV{'HTTP_REFERER'}\">Back</a> <a href=\"$boardLocation?board=$FORM{'board'}&action=index\">Message Index</a>");
		exit;
    }
}

#
###

### Gets all the cookies into a hash:
#
# Note:  a "mega-cookie" essentially looks like this when it is stored:
# megaCookieName=cookieName<>cookieValue&cookieName2<>cookieValue2; 
# The cookieName and cookieValue pairs are actually the data we want, and
# their special characters are hex-encoded.

sub getCookies {

  #Determine the names of the cookies wanted:
  my $cookieName = &cookieEncode($FORM{'board'});
  my $tempCookieName = $cookieName . "Temp";

  #Get all mega-cookies:
  my $CookieIn = $ENV{'HTTP_COOKIE'};
  
  #Split the mega-cookies into an array, each value being a mega-cookie:
  my @startcookies = split(/; /, $CookieIn);

  #Put the mega-cookies into a hash with the names and values.  Names
  #are the names set initially in this program with $cookieName and
  #$tempCookieName.

  my %INITCOOKIES;

  foreach my $startcookie (@startcookies) {
    (my $name, my $value) = split(/=/, $startcookie);
    if ($name eq $cookieName || $name eq $tempCookieName) {
	$INITCOOKIES{$name} = $value;
    }
  }

  #For each value of a mega-cookie (which is a string of names and values,
  #which are what we actually want), split the names and values into
  #an array.  Then, split the names from the values inside this array, insert
  #them into the COOKIES hash, and go onto the next mega-cookie.

  foreach my $cookiestring (values %INITCOOKIES) {
    my @cookies = split(/&/, $cookiestring);
    foreach my $cookie (@cookies) {
	(my $name, my $value) = split(/<>/, $cookie);
	$name = &cookieDecode($name);
	$value = &cookieDecode($value);
	$COOKIES{$name} = $value;
    } 
  }
}

#
###

### Sets up complicated cookies initialization:
#

sub initCookies {

    my $writelastvisit = time;

  # If the user is not yet logged in, but has visited previously,
  # set the temp cookie to all of the info that was permanently saved the
  # last time; this info will now be used for reference against the last 
  # time since it will not be changed for this user's session. The Name
  # value is added regardless of whether it is null or not. email too
    
    if ($COOKIES{'LoggedIn'} ne '1' && $COOKIES{'LastNum'}) {
	&setCookie(1, "NumTemp", $COOKIES{'LastNum'}, "VisitTemp", $COOKIES{'LastVisit'}, "LoggedIn", 1);

	$COOKIES{'NumTemp'} = $COOKIES{'LastNum'};
	$COOKIES{'VisitTemp'} = $COOKIES{'LastVisit'};

	&setCookie(0, "LastVisit", $writelastvisit, "LastNum", $num, "Name", $COOKIES{'Name'}, "email", $COOKIES{'email'});

	$COOKIES{'LoggedIn'} = '1';
    }
    
   # ...but if the user is not logged in and has NOT visited previously,
   # the previous values will be set to the actual current values, not 
   # using values from an old cookie. Name will be set to null. email too
    
    elsif ($COOKIES{'LoggedIn'} ne '1') {
	&setCookie(0, "LastVisit", $writelastvisit, "LastNum", $num, "Name", '', "email", '');

	&setCookie(1, "LoggedIn", 1, "NumTemp", $num, "VisitTemp", $writelastvisit);
	
	$COOKIES{'NumTemp'} = $num;
	$COOKIES{'VisitTemp'} = $writelastvisit; 
	$COOKIES{'LoggedIn'} = '1';
    }
    
   # Find the number of posts that have been added since NumTemp was saved
    
    if ($COOKIES{'NumTemp'} || $COOKIES{'NumTemp'} == 0) {
	$Diff = $num - $COOKIES{'NumTemp'};
    }
}
#
###

### Prints a welcome message if cookies are enabled:
#

sub cookieWelcome {
    print "<p>\n";

  # If the user has provided a name, address the user with their name:

    if ($COOKIES{'VisitTemp'} && $COOKIES{'Name'}) {
	my $lastdate = &niceTime($COOKIES{'VisitTemp'});
	print <<EndOfHTML;
Welcome, $COOKIES{'Name'}, your last visit was on $lastdate
and there have been $Diff posts added since then.
EndOfHTML
    }

  # If the user has not provided a name:

    elsif ($COOKIES{'VisitTemp'}) {
	my $lastdate = &niceTime($COOKIES{'VisitTemp'});
	print <<EndOfHTML;
Welcome, your last visit was on $lastdate and there have been
$Diff posts added since then.
EndOfHTML

    } 
    print "</p>\n";
}

#
###

### Prints a post to MSGOUT in the current position in the parse:
#

sub writePost {
    my $date = time;
    &indent;
    print MSGOUT "<post id=\"$num\">\n";  &indent;
    print MSGOUT "  <name><![CDATA[$FORM{'name'}]]></name>\n";  &indent;
    print MSGOUT "  <date>$date</date>\n";  &indent;
    print MSGOUT "  <email><![CDATA[$FORM{'email'}]]></email>\n";  &indent;
    print MSGOUT "  <ip>$ENV{'REMOTE_ADDR'}</ip>\n";  &indent;
    print MSGOUT "  <subject><![CDATA[$FORM{'subject'}]]></subject>\n";  &indent;
    print MSGOUT "  <message><![CDATA[$FORM{$MessageTextArea}]]></message>\n";  &indent;
    print MSGOUT "  <link><![CDATA[$FORM{'link'}]]></link>\n";  &indent;
    print MSGOUT "</post>\n";
}

#
###

### Prints out spaces for indentation in the write action:
#

sub indent {

    my $indentDepth = $globalDepth;

  # For removeold action, indent less if posts have been skipped:

    my $difference = 0;
    foreach my $k (keys %skippedAtDepth) {
	$difference++;
    }
    $indentDepth -= $difference;


    for (my $i = 0; $i < $indentDepth; $i++) {
	print MSGOUT '  ';
    }
}

#
###

### Prints a pretty page showing the boards that exists:
#
# Feel free to change the design on this, make it more minimalist, less...
# green...  or whatever.
# This subroutine does not have html files to read from because no valid
# boards were specified, so I just had some fun with my own design.

sub noBoard {
    print "Content-type:text/html\n\n";
    print <<EndOfHTML;

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Final//EN">
<html>
<head>
<style type="text/css">
<!--
    body {font-family : Arial;}
    td.border {border-color : 006600; border-style : solid;}
    h3 {font-variant : small-caps; color : 006600}
    p {font-size : x-small;
       letter-spacing : 2px; 
      }
    a:link, a:active, a:hover, a:visited {text-decoration : none;
       font-weight : bold;
    }
-->
</style>
<title>XMLBoard</title>
</head>
<body bgcolor="#009966" color="#006600" link="#006600" alink="#009966" vlink="#006600">
<table width="100%" height="400" border="0" cellspacing="0" cellpadding="0">
<tr>
<td valign="middle">

<table width="300" height="200" cellspacing="0" cellpadding="0" align="center">
<tr>
<td valign="middle" bgcolor="#33FF00" class="border">
<center>
<h3>Nonexistent or Invalid Board</h3>
<p>You need to specify a valid board.  Here are the existing boards:</p>
EndOfHTML

    my $dash = 0;
    foreach my $board (keys %boards) {
	if ($dash) {
	    print "\|\n";
	}
	print "<a href=\"$boardLocation?board=$board&action=index\">$board</a>\n";
	$dash = 1;
    }

    print <<EndOfHTML;
</center>
</td>
</tr>
</table>

</td>
</tr>
</table>
</body>
</html>
EndOfHTML

}

#
###

### Sets multiple cookies compressed into one:
#

sub setCookie {

    my ($tempSwitch, @cookies) = @_;
    my $cookieValue;
    my $cookieName = "$FORM{'board'}";

  # If $tempSwitch is true, add "temp" to the end of the cookie name.

    if ($tempSwitch) {
	$cookieName .= "Temp";
    }

    $cookieName = &cookieEncode($cookieName);

  # Format the string of name/value pairs, while converting all invalid
  # characters:

    while ((my $name, my $value) = @cookies) {

	$name = &cookieEncode($name);
	$value = &cookieEncode($value);
	
 	if ($cookieValue) {
	    $cookieValue .= "\&$name<>$value";
	}
	else {
	    $cookieValue = "$name<>$value";
	}

  # Remove the name and value pair just used from the array, so that the
  # while loop can look at the next pair (and not go into an infinite loop).

	shift(@cookies); shift(@cookies);
    }

  # Print out the cookie:

    if ($tempSwitch) {
	print "Set-Cookie: $cookieName=$cookieValue\;\n";
    }
    else {
	my $expires = localtime(time + $expiresIn);
	print "Set-Cookie: $cookieName=$cookieValue\; expires=$expires\;\n";
    }

}

#
###

### Encode a variable for cookie usage:
#

sub cookieEncode {
    my ($var) = @_;
    $var =~ s/\%/\%25/g;
    $var =~ s/\+/\%2B/g;
    $var =~ s/\;/\%3B/g;
    $var =~ s/\,/\%2C/g;
    $var =~ s/\=/\%3D/g;
    $var =~ s/\&/\%26/g;
    return $var;
}

#
###

### Decode a variable for cookie usage:
#

sub cookieDecode {
    my ($var) = @_;
    $var =~ s/\%26/\&/g;
    $var =~ s/\%3D/\=/g;
    $var =~ s/\%2C/\,/g;
    $var =~ s/\%3B/\;/g;
    $var =~ s/\%2B/\+/g;
    $var =~ s/\%25/\%/g;
    return $var;
}

#
###

### Check the administrator's name and password:
#

sub checkPass {

    open (PASSIN, "$boards{$FORM{'board'}}/pass.txt");
    my $line = <PASSIN>;
    close (PASSIN);
    chomp($line);
    my ($name, $pass) = split(/:/, $line);

  # If the name given doesn't match the name in the file or the password given
  # doesn't match the name in the file, print a message and exit.  The code for
  # encrypting the password according to its first two characters was taken
  # from Matt's WWWBoard Admin script... because it is good.

  # Trim whitespace:

    $FORM{'adminName'} = &trim($FORM{'adminName'});
    $FORM{'adminPass'} = &trim($FORM{'adminPass'});

    if ($FORM{'adminName'} ne $name || crypt($FORM{'adminPass'}, substr($FORM{'adminPass'}, 0, 2)) ne $pass) {
	&message("Invalid Name/Password", "<h3>Invalid Name/Password</h3><p>The name and password combination you have given is invalid.</p><a href=\"$boardLocation?board=$FORM{'board'}&action=index\">Message Index</a>");
	exit;
    }
}


#
###

### Change password:
#

sub changePass {

  # Trim whitespace:

    $FORM{'newAdminName'} = &trim($FORM{'newAdminName'});
    $FORM{'newAdminPass'} = &trim($FORM{'newAdminPass'});
    $FORM{'newAdminPassCheck'} = &trim($FORM{'newAdminPassCheck'});

  # If the name doesn't exist:

    if (! $FORM{'newAdminName'}) {
	&message("Invalid Name", "<h3>Invalid Name</h3><p>You must fill in a valid name.</p><a href=\"$ENV{'HTTP_REFERER'}\">Back</a> <a href=\"$boardLocation?board=$FORM{'board'}&action=index\">Message Index</a>");
    }

  # If the name contains colons:

    elsif ($FORM{'newAdminName'} =~ m/:/) {
	&message("Invalid Name", "<h3>Invalid Name</h3><p>You may not have colons (:) in your name.</p><a href=\"$ENV{'HTTP_REFERER'}\">Back</a> <a href=\"$boardLocation?board=$FORM{'board'}&action=index\">Message Index</a>");
    }

  # If the password is less than 4 characters:

    elsif (length($FORM{'newAdminPass'}) < 4) {
	&message("Invalid Password", "<h3>Invalid Password</h3><p>Your new password must be at least 4 characters long.</p><a href=\"$ENV{'HTTP_REFERER'}\">Back</a> <a href=\"$boardLocation?board=$FORM{'board'}&action=index\">Message Index</a>");
    }

  # If the retyped password does not match the first password:

    elsif ($FORM{'newAdminPass'} ne $FORM{'newAdminPassCheck'}) {
	&message("Invalid Password", "<h3>Invalid Password</h3><p>The second password did not match the first password.</p><a href=\"$ENV{'HTTP_REFERER'}\">Back</a> <a href=\"$boardLocation?board=$FORM{'board'}&action=index\">Message Index</a>");
    }

  # Change the password:

    else {
	open (PASSOUT, ">$boards{$FORM{'board'}}/pass.txt");
	flock(PASSOUT, LOCK_EX);
	my $newPass = crypt($FORM{'newAdminPass'}, substr($FORM{'newAdminPass'}, 0, 2));
	print PASSOUT "$FORM{'newAdminName'}:$newPass\n";
	flock(MSGOUT, LOCK_UN);
	close (PASSOUT);
	&message("Name/Password Changed", "<h3>Name/Password Changed</h3><p>Your name and/or password has been changed.</p><a href=\"$boardLocation?board=$FORM{'board'}&action=index\">Message Index</a>");
    }

}

#
###

### Trim whitespace off the ends of strings:
#
# Note: Most importantly, this keeps users from entering names such as 
# "   ", because this function will turn them into null strings, which
# evaluate as false.

sub trim {
    my ($string) = @_;
    $string =~ s/^\s+//;
    $string =~ s/\s+$//;
    return $string;
}

#
###

### Show a little list of links:
#

sub showLinks {

    print "<a href=\"$ENV{'HTTP_REFERER'}\">Back</a>\n";

    if ($FORM{'action'} eq "search" || $FORM{'action'} eq "read") {
	print "\| <a href=\"$boardLocation?board=$FORM{'board'}&action=index\">Message Index</a>\n";
    }

    if ($FORM{'action'} eq "index" || $FORM{'action'} eq "read") {

  # If specified in the configuration, a link will be shown, pointing to the
  # "searchpage" action:

	if ($searchLink) {
	    print "\| <a href=\"$boardLocation?board=$FORM{'board'}&action=searchpage\">Search</a>\n";
	}
    }

  # If specified in the configuration, a link will be shown, pointing
  # to the "admin" action:

    if ($adminLink) {
	print "\| <a href=\"$boardLocation?board=$FORM{'board'}&action=admin\">Admin</a>\n";
    }

}

#
###

### Filter naughty words from a string.
#

sub playNice {
    my ($badString) = @_;
    my %badWords = (fucker => 'lover',
		    pussy => 'friend',
		    bitch => 'person',
		    damn => 'darn',
		    hell => 'heck',
		    fag => 'homosexual',
		    fuck => 'love',
		    shit => 'love');
    foreach my $badWord (keys %badWords) {
	$badString =~ s/$badWord/$badWords{$badWord}/ig;
    }
    return $badString;

}

#
###


### Parse the file:
#
# This parser is my hand-made replacement to XML::Parser.
# It reads the file line by line, and uses a large regexp on each line
# to determine what is in the line.  The regexp looks for (in this order)
# 0 or 1 start tag with 0 or 1 name/value attribute, 0 or 1 open CDATA tag,
# 0 or 1 string of data, 0 or 1 close CDATA tag, and 0 or 1 close tag. All
# of the matches the regexp gets are put into 10 variables, 5 of which are 
# used.
# In this parser, $globalDepth, @globalInElement, and $globalWithinElementPost
# are replacement variables for values that would otherwise be found using
# a function of XML::Parser.

sub ParseFile {

    my ($INFILE) = @_;

  # Open the file in read mode for parsing:

  # Start a loop that sets $lineInParse equal to the contents of the next
  # line in INFILE.  The loop will end when it reaches EOF.

    while ((my $lineInParse = <$INFILE>) && (! $stopParse)) {

	if ($lineInParse =~ m/^\s*(<([A-Za-z]+)(\ (.*)=\"(.*)\")?>)?\s*(<\!\[CDATA\[)?(.*?)?(\]\]>)?(<\/([A-Za-z]+)>)?\s*$/) { 
	    my $startType = $2;
	    my $attName = $4;
	    my $attVal = $5;
	    my $data = $7;
	    my $endType = $10;

  # If there's data at the end of the line, or nothing was found in the line,
  # add a newline to the data.

	    if (($data && !$endType) || (!$startType && !$data && !$endType)) {
		$data .= "\n";
	    }

  # If the line has a start tag in it, call the start tag handler.

	    if ($startType) {
		if ($FORM{'action'} eq "index") {
		    &index_handle_start($startType, $attName, $attVal);
		}
		elsif ($FORM{'action'} eq "read") {
		    &read_handle_start($startType, $attName, $attVal);
		}
		elsif ($FORM{'action'} eq "write") {
		    &write_handle_start($startType, $attName, $attVal);
		}
		elsif ($FORM{'action'} eq "remove") {
		    &remove_handle_start($startType, $attName, $attVal);
		}
		elsif ($FORM{'action'} eq "removeold") {
		    &removeold_handle_start($startType, $attName, $attVal);
		}
		elsif ($FORM{'action'} eq "search") {
		    &search_handle_start($startType, $attName, $attVal);
		}
		$globalDepth++;
		$globalInElement[$globalDepth] = $startType;
	    }
	    
  # If the line has character data in it, call the character data handler.

	    if ($data) {
		if ($FORM{'action'} eq "index") {
		    &index_handle_char($data);
		}
		elsif ($FORM{'action'} eq "read") {
		    &read_handle_char($data);
		}
		elsif ($FORM{'action'} eq "write") {
		    &write_handle_char($data);
		}
		elsif ($FORM{'action'} eq "remove") {
		    &remove_handle_char($data);
		}
		elsif ($FORM{'action'} eq "removeold") {
		    &removeold_handle_char($data);
		}
		elsif ($FORM{'action'} eq "search") {
		    &search_handle_char($data);
		}
	    }

  # If the line has an end tag in it, call the end element hander:
	    if ($endType) {	
		$globalInElement[$globalDepth] = '';
		$globalDepth--;
		if ($FORM{'action'} eq "index") {
		    &index_handle_end($endType);
		}
		elsif ($FORM{'action'} eq "read") {
		    &read_handle_end($endType);
		}
		elsif ($FORM{'action'} eq "write") {
		    &write_handle_end($endType);
		}
		elsif ($FORM{'action'} eq "remove") {
		    &remove_handle_end($endType);
		}
		elsif ($FORM{'action'} eq "removeold") {
		    &removeold_handle_end($endType);
		}
		elsif ($FORM{'action'} eq "search") {
		    &search_handle_end($endType);
		}
	    }
	}
    }
}

#
###

### Call subroutines:
#


&getForm;    #get GET or POST values into the %FORM hash

if (! exists $boards{$FORM{'board'}}) {    #if the board requested does
    &noBoard;                             #not exist, complain
    exit;
}

do "$boards{$FORM{'board'}}/forum.conf";  # Run configuration for the forum.

  # The changepw action and searchpage action exit the script at this point
  # because they are the only actions that do not involve any XML parsing:

if ($FORM{'action'} eq "changepw") {
    &checkPass;
    &changePass;
    exit;
}

if ($FORM{'action'} eq "searchpage") {

  # Run the message function with the search form as contents and exit:

    &message("Search", "<a href=\"$ENV{'HTTP_REFERER'}\">Back</a>
 \| <a href=\"$boardLocation?board=$FORM{'board'}&action=index\">Message Index</a>
<form action=\"$boardLocation\" method=\"post\">
<table><tr><td>
<h1>Search</h1></td></tr>
<tr><td><input size=\"$fieldLength\" name=\"searchString\"></td></tr>
<tr><td><input type=\"checkbox\" name=\"caseSensitive\" value=\"1\"> Case
Sensitive</td></tr>
<tr><td>Match <select name=\"matchType\" size=\"1\">
<option value=\"phrase\" selected>Entire Phrase</option>
<option value=\"any\">Any Word</option>
<option value=\"all\">All Words</option>
</select></td></tr>
<tr><td>Search In <ul><li>
<input type=\"checkbox\" name=\"checkName\" value=\"1\" checked>Author's Name
</li>
<li><input type=\"checkbox\" name=\"checkSubject\" value=\"1\" checked>Subject
</li>
<li><input type=\"checkbox\" name=\"checkMessage\" value=\"1\" checked>Message
</li></ul></td></tr>
<tr><td><input type=\"hidden\" name=\"board\" value=\"$FORM{'board'}\">
<input type=\"hidden\" name=\"action\" value=\"search\">
<input type=\"submit\" value=\"Search\">
</td></tr></table></form>");
    exit;
}

  # If the action requested is not valid...

if ($FORM{'action'} ne "index" && $FORM{'action'} ne "read" && $FORM{'action'} ne "write" && $FORM{'action'} ne "admin" && $FORM{'action'} ne "remove" && $FORM{'action'} ne "removeold" && $FORM{'action'} ne "changepw" && $FORM{'action'} ne "search" && $FORM{'action'} ne "searchpage") {
    $FORM{'action'} = "index";       #the action is set to index
}

if ($FORM{'action'} eq "admin") {  # if the action is admin, have the script
    $FORM{'action'} = "index";     # do an index, but set a variable that
    $admin = 1;                    # tells the script to put radio buttons
}                                  # next to posts, and put a different form
                                   # at the bottom of the page.

&setTags($FORM{'action'});    #set up the tags wanted for the action


if ($useCookies) {    #get cookies if they are enabled
    &getCookies;
}

### Setup for each action:

# For the index action:

if ($FORM{'action'} eq "index") {
    $xml_file = "msg.xml";
    open (MSGIN, "$boards{$FORM{'board'}}/$xml_file"); flock(MSGIN, LOCK_SH); &ParseFile(*MSGIN); close(MSGIN);

    &printForm($FORM{'action'});   #prints out the appropriate form for posting
    &htmlBottom;    #print the bottom of the page
}

# For the read action:

elsif ($FORM{'action'} eq "read") {
    $requestedId = $FORM{'id'};  #remember which id was initially requested
    $xml_file = "msg.xml";
    open (MSGIN, "$boards{$FORM{'board'}}/$xml_file"); flock(MSGIN, LOCK_SH); &ParseFile(*MSGIN); close(MSGIN);
    if (! $postExists) {
	print "Location:$boardLocation?board=$FORM{'board'}&action=index\n\n";
	exit;   #if the requested post does not exists, redirect to the index
    }
    if ($checkTwice) {    #sometimes the read action needs to parse twice
	$secondCheck = 1;  #let the handlers know this is the second parsing
	$stopParse = 0;
	open (MSGIN, "$boards{$FORM{'board'}}/$xml_file"); flock(MSGIN, LOCK_SH); &ParseFile(*MSGIN); close(MSGIN);
    }

  # Set up for a mini index:

    $FORM{'action'} = "index";
    $globalDepth = 0;
    $globalWithinElementPost = 0;
    @globalInElement = '';
    &setTags($FORM{'action'});             #show an index of followups

    if ($uList) {       # If writing an unordered list, format accordingly
	print "<ul>\n";
    }
    else {
	print "<table border=\"$border\" cellspacing=\"$cellSpacing\" $cellpadding=\"$cellPadding\">\n";
    }


    $stopParse = 0;
    open (MSGIN, "$boards{$FORM{'board'}}/$xml_file"); flock(MSGIN, LOCK_SH); &ParseFile(*MSGIN); close(MSGIN);

  # If there are no responses to the post, print a message:

    if (! $existResponses) {
	print "(No Responses Posted)\n";
    }
    &printForm("read");   #prints out the appropriate form for posting
    &htmlBottom;    #print the bottom of the page
}

# For the write action:

elsif ($FORM{'action'} eq "write") {
    &checkData;    #make sure the user has provided necessary data

  #Note: for now, msg.xml is referred to as MSGOUT even when it is being read
  #from, because I'm trying to fix a bug.  Yay! 

    open (MSGOUT, "+<$boards{$FORM{'board'}}/msg.xml");
    flock (MSGOUT, LOCK_EX);
    open (MSGTMP, "+>$boards{$FORM{'board'}}/msg.temp");
    flock (MSGTMP, LOCK_EX);
    copy (*MSGOUT, *MSGTMP);
    truncate (MSGOUT, 0);
    seek (MSGOUT,0,0);
    seek (MSGTMP,0,0);

    &ParseFile(*MSGTMP);

    if (! $leaveTemp) 
	{        				#if leaveTemp is false, delete the temp file
		unlink("$boards{$FORM{'board'}}/msg.temp");
    }

    close (MSGOUT);
    close (MSGTMP);


	send_admin_note($FORM{'name'}." wrote\n".$FORM{$MessageTextArea});



    &message("Thank You", "<h3>Thank You</h3><p>Your message has been posted.</p><a href =\"$boardLocation?board=$FORM{'board'}&action=read&id=$num\">Review Your Post</a> <a href = \"$boardLocation?board=$FORM{'board'}&action=index\">Message Index</a>");


}

# For the remove action:

elsif ($FORM{'action'} eq "remove") {
    &checkPass;   #make sure the user is an administrator
    &checkData;   #make sure the form data is good

    open (MSGOUT, "+<$boards{$FORM{'board'}}/msg.xml");
    flock (MSGOUT, LOCK_EX);
    open (MSGTMP, "+>$boards{$FORM{'board'}}/msg.temp");
    flock (MSGTMP, LOCK_EX);
    copy (*MSGOUT, *MSGTMP);
    truncate (MSGOUT, 0);
    seek (MSGOUT,0,0);
    seek (MSGTMP,0,0);

    &ParseFile(*MSGTMP);

    if (! $leaveTemp) {        #if leaveTemp is false, delete the temp file
	unlink("$boards{$FORM{'board'}}/msg.temp");
    }

    close (MSGOUT);
    close (MSGTMP);


    &message("Posts Removed", "<h3>Posts Removed</h3><p>The posts you have specified have been removed.</p><a href=\"$boardLocation?board=$FORM{'board'}&action=index\">Message Index</a>");
}

# For the removeold action:

elsif ($FORM{'action'} eq "removeold") {
    &checkPass;    #make sure the user is an administrator
    &checkData;    #make sure the form data is good

    open (MSGOUT, "+<$boards{$FORM{'board'}}/msg.xml");
    flock (MSGOUT, LOCK_EX);
    open (MSGTMP, "+>$boards{$FORM{'board'}}/msg.temp");
    flock (MSGTMP, LOCK_EX);
    copy (*MSGOUT, *MSGTMP);
    truncate (MSGOUT, 0);
    seek (MSGOUT,0,0);
    seek (MSGTMP,0,0);

    &ParseFile(*MSGTMP);

    if (! $leaveTemp) {        #if leaveTemp is false, delete the temp file
	unlink("$boards{$FORM{'board'}}/msg.temp");
    }

    close (MSGOUT);
    close (MSGTMP);


    &message("Old Posts Removed", "<h3>Old Posts Removed</h3><p>Posts more than $FORM{'rmAge'} seconds old have been removed.</p><a href=\"$boardLocation?board=$FORM{'board'}&action=index\">Message Index</a>");
}

# For the search action:

elsif ($FORM{'action'} eq "search") {

  # If not matching a phrase, split the words up:

    if ($FORM{'matchType'} eq "any" || $FORM{'matchType'} eq "all") {
	@searchKeys = split(/\s+/, $FORM{'searchString'});
    }
    else {
	@searchKeys = $FORM{'searchString'};
    }

    $xml_file = "msg.xml";
    &htmlTop($FORM{'action'});    #print the top of the page
    &showLinks;   #print some links

    print "<h1>Search Results</h1>\n";

    if ($uList) {       # If writing an unordered list, format accordingly
	print "<ul>\n";
    }
    else {
	print "<table border=\"$border\" cellspacing=\"$cellSpacing\" cellpadding=\"$cellPadding\">\n";
    }

    open (MSGIN, "$boards{$FORM{'board'}}/$xml_file"); flock(MSGIN, LOCK_SH); &ParseFile(*MSGIN); close(MSGIN);


    if ($uList) {

	if (!$searchPassed) {
	    print "<li>No Matches Found</li>\n";
	}
	print "</ul>\n";
    }
    else {
	if (!$searchPassed) {
	    print "<tr><td>No Matches Found</td></tr>\n";
	}
	print "</table>\n";
    }

    &htmlBottom;    #print the bottom of the page
}

#
###



