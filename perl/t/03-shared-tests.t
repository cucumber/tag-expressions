#!perl


use strict;
use warnings;

use Test2::V0;
use Test2::Tools::Exception qw(lives dies);
use YAML qw(LoadFile);


use Cucumber::TagExpressions;


my $cases = LoadFile('../testdata/evaluations.yml');

for my $case ( @{ $cases } ) {
    my $evaluator;
    ok(
        lives {
            $evaluator = Cucumber::TagExpressions->parse( $case->{expression} );
        },
        qq{Parsing "$case->{expression}"} )
        or diag($@);

    ok( $evaluator, "Have an evaluator object from the parser" );
    if ($evaluator) {
        for my $test ( @{ $case->{tests} } ) {
            my $result = $evaluator->evaluate( @{ $test->{variables} } );
            is( $result ? "true" : "false",
                $test->{result},
                "Evaluating $case->{expression} with variables @{$test->{variables}}" );
        }
    }
}


$cases = LoadFile('../testdata/errors.yml');

for my $case ( @{ $cases } ) {
    like(
        dies {
            Cucumber::TagExpressions->parse( $case->{expression} );
        },
        qr/\Q$case->{error}\E/,
        qq{Parsing "$case->{expression}"} );
}


$cases = LoadFile('../testdata/parsing.yml');

for my $case ( @{ $cases } ) {
    my $evaluator;
    ok(
        lives {
            $evaluator = Cucumber::TagExpressions->parse( $case->{expression} );
        },
        qq{Parsing "$case->{expression}"} )
        or diag($@);

    is( $evaluator->stringify,
        $case->{formatted},
        "Stringified parser for $case->{expression}" );
}

done_testing;
