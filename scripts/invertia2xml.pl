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

$starting_line = "<b>Ticker</b>";
#$data_pattern = "fichavalor.asp[?]isin=([^\"]+)\">[^>]*> (.*)</A></TD><TD>([^<]+)</TD><TD[^<]+</TD><TD[^<]+</TD><TD[^<]+</TD><TD>([^<]+)</TD><TD>([^<]+)</TD><TD[^<]+</TD><TD[^>]*>([^<]+)</TD></TR>";
$data_pattern = "empresa.asp[?]idtel=[^=]+='([^']*)'>([^<]+)</a></td><td[^>]*>([^&]+)&nbsp;</td><td[^<]+</td><td[^<]+</td><td[^>]+>(<img[^>]+>)*</td><td[^<]+</td><td[^<]+</td><td[^>]*>([^&]+)&nbsp;</td><td[^>]*>([^<]+)";
#empresa.asp?idtel=RV011AUCESA title='ABERTIS'>ABE</a></td><td CLASS=TABLA NOWRAP>12,215&nbsp;</td><td CLASS=TABLA NOWRAP>-0,120&nbsp;</td><td CLASS=TABLA NOWRAP  >-1,0%</td><td align=right NOWRAP class=TABLA><img src=/img/i/pr.gif width=5,076 height=10><img src=/img/i/pt.gif width=39,984 height=10></td><td CLASS=TABLA>12,460&nbsp;</td><td CLASS=TABLA>12,090&nbsp;</td><td CLASS=TABLA  >767.505&nbsp;</td><td CLASS=TABLA width=60  >9.027<td class=TABLA NOWRAP  >4,91%</td><td class=TABLA NOWRAP  > 13,68</td>
#$value_pattern = "<a href=\"\/bolsa\/cotizaciones\/Ficha[?]cod=([^\"]+)\"[^>]+>(.*)<\/a>";
$time_pattern = "^<td CLASS=TABLA>&nbsp;(.+)";
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
        if (/$data_pattern/) # code, name, price, volume, capital, time
        {
            if ($state != 1)
            {
                print "</value>\n";
            }
            print "<value>";
            &put_str("M.$2", "code");
            &put_str($1, "name");
            &put_num($3, "price");
            &put_num($5, "volume");
            &put_num($6, "capital");
            $state = 9;
        }
        elsif ($state == 9)
	{
		if (/$time_pattern/)
		{
		    print "<time>$1</time>";
		    $state++;
		}
	}
    }
}

if ($state != 0)
{
    if ($state != 1)
    {
        print "</value>\n";
    }
    print "</ccltor_xml_data>\n";
}
