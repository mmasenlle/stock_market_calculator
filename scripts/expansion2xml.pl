#!/usr/bin/perl

# ccltor parser for the url << http://app2.expansion.com/bolsa/cotizaciones/Ficha?cod=I.MA >>

sub put_col
{
    $num = $_[0];
    $num =~ s/\.//g;
    $num =~ s/,/\./;
    print "<$_[1]>$num</$_[1]>";
}

$starting_line = "<DIV id=\"resultados_buscador\">";
$value_pattern = "<a href=\"\/bolsa\/cotizaciones\/Ficha[?]cod=([^\"]+)\"[^>]+>(.*)<\/a>";
$col_pattern = "<td.*>(.+)</td>";
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
        if (/$value_pattern/) # code, name
        {
            if ($state != 1)
            {
                print "</value>\n";
            }
            print "<value><code>$1</code><name>$2</name>";
            $state = 2;
        }
        elsif (/$col_pattern/)
        {
            if ($state == 2) # price
            {
                &put_col($1, "price");
                $state++;
            }
            elsif ($state == 3) { $state++; } # diffp
            elsif ($state == 4) { $state++; } # diff
            elsif ($state == 5) { $state++; } # max
            elsif ($state == 6) { $state++; } # min
            elsif ($state == 7) # volume
            {
                &put_col($1, "volume");
                $state++;
            }
            elsif ($state == 8) # capital
            {
                &put_col($1, "capital");
                $state++;
            }
            elsif ($state == 9) # time
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
