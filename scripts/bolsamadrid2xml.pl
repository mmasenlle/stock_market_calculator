#!/usr/bin/perl

# ccltor parser for the url << http://www.bolsamadrid.es/esp/mercados/acciones/accmerc2_c.htm >>

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

$starting_line = "<TD ID=Tit>Hora</TD>";
$data_pattern = "fichavalor.asp[?]isin=([^\"]+)\">[^>]*> (.*)</A></TD><TD>([^<]+)</TD><TD[^<]+</TD><TD[^<]+</TD><TD[^<]+</TD><TD>([^<]+)</TD><TD>([^<]+)</TD><TD[^<]+</TD><TD[^>]*>([^<]+)</TD></TR>";
#fichavalor.asp?isin=ES0105200416"><IMG SRC="/images/arr-dw9.gif" BORDER=0> ABENGOA</A></TD><TD>16,4300</TD><TD ID=R>-1,59</TD><TD>16,8900</TD><TD>16,3950</TD><TD>201.662</TD><TD>3.347,65</TD><TD align=center>16/06/2010</TD><TD align=center>10:18</TD></TR>
#$value_pattern = "<a href=\"\/bolsa\/cotizaciones\/Ficha[?]cod=([^\"]+)\"[^>]+>(.*)<\/a>";
#$col_pattern = "<td.*>(.+)</td>";
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
            &put_str($1, "code");
            &put_str($2, "name");
            &put_num($3, "price");
            &put_num($4, "volume");
            &put_num($5, "capital");
            print "<time>$6</time>";
            $state = 10;
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
