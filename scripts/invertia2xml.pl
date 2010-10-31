#!/usr/bin/perl

# ccltor parser for the url << http://www.invertia.com/mercados/acciones/default.asp?idtel=IB011CONTINU >>

sub put_str
{
    $str = $_[0];
    $str =~ s/'/\\'/g;
    print "<$_[1]>$str</$_[1]>";
}

sub put_num
{
    $num = $_[0];
    $num =~ s/\.//g;
    $num =~ s/,/\./;
    print "<$_[1]>$num</$_[1]>";
}

#$starting_line = "<b>Ticker</b>";
$starting_line = "<table title=\"Acciones\"";
#portada.asp[?]idtel=RV011A3TV" title="ANTENA 3">A3TV</a>
$value_pattern = "portada.asp[?]idtel=.+title=\"([^\"]+)\">(.*)<\/a>";
#$data_pattern = "empresa.asp[?]idtel=[^=]+='([^']*)'>([^<]+)</a></td><td[^>]*>([^&]+)&nbsp;</td><td[^<]+</td><td[^<]+</td><td[^>]+>(<img[^>]+>)*</td><td[^<]+</td><td[^<]+</td><td[^>]*>([^&]+)&nbsp;</td><td[^>]*>([^<]+)";
$data_pattern = "<td>([^<]+)</td><td>[^<]*</td><td>[^<]*</td><td><img[^>]*><img[^>]*></td><td>[^<]*</td><td>[^<]*</td><td>([^<]+)</td><td>([^<]+)</td>";
#<td>6.79</td><td>0.20</td><td>3.0</td><td><img src=/images/pv.gif alt="Gr&aacute;fico" width=2.22 height=9><img src=/images/pt.gif alt="Gr&aacute;fico" width=29.52 height=9></td><td>6.89</td><td>6.55</td><td>488,863</td><td>1,433</td><td>5.60&nbsp;%</td><td> 14.02</td>
$time_pattern = "<td>([^<]+)</td>";
$state = 0; # state = { init, value, price,  diffp, diff, max, min, volume, capital, time,  end }

foreach $_ (<STDIN>)
{
    if ($state == 0) # init
    {
        if (/$starting_line/)
        {
            print "<ccltor_xml_data>\n";
            $state++;
        }
    }
    else
    {
		if ($state == 1)
		{
		    if (/$value_pattern/) # name, code
			{
				print "<value>";
				&put_str("M.$2", "code");
				&put_str($1, "name");
				$state = 3;
			}
		}
		elsif ($state == 3)
        {
            if (/$data_pattern/) # price, volume, capital
            {
				&put_num($1, "price");
				&put_num($2, "volume");
				&put_num($3, "capital");
				$state = 6;
			}
        }
        elsif ($state == 6)
		{
			if (/$time_pattern/)
			{
				print "<time>$1</time>";
				print "</value>\n";
				$state = 1;
			}
		}
    }
}

if ($state != 0)
{
    print "</ccltor_xml_data>\n";
}
