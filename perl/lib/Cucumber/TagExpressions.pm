
package Cucumber::TagExpressions;

=head1 NAME

Cucumber::TagExpressions - Tag expression parser

=head1 SYNOPSIS

  use Cucumber::TagExpressions;

  my $expr = Cucumber::TagExpressions->parse( '@a and @b' );
  if ( $expr->evaluate( qw/x y z/ ) ) {
     say "The evaluation returned false";
  }


=head1 DESCRIPTION

Cucumber tag expressions allow users to define the subset of Gherkin
scenarios they want to run. This library parses the expression and
returns an evaluator object which can be used to test the tags specified
on a scenario against the filter expression.

=head1 CLASS METHODS

=cut

use strict;
use warnings;

use Cucumber::TagExpressions::Node;

sub _expect_token {
    my ( $state, $token ) = @_;

    my $actual = _get_token( $state );
    die "Expecting token '$token' but found '$actual'"
        if $token ne $actual;
}

sub _consume_char {
    my ( $state, $allow_eof ) = @_;

    if ( length($state->{text}) <= $state->{pos} ) {
        return if $allow_eof;
        die "Unexpected end of string parsing tag expression: $state->{text}";
    }
    return substr( $state->{text}, $state->{pos}++, 1 );
}

sub _get_token {
    my ( $state ) = @_;

    return delete $state->{saved_token} if defined $state->{saved_token};

    my $token = '';
    while (1) {
        my $char = _consume_char( $state, 1 );
        return ($token ? $token : undef)
            if not defined $char;

        if ( $char =~ m/\s/ ) {
            if ( $token ) {
                return $token;
            }
            else {
                next;
            }
        }
        elsif ( $char eq '(' or $char eq ')' ) {
            if ( $token ) {
                _save_token( $state, $char );
                return $token;
            }
            else {
                return $char;
            }
        }
        if ( $char eq "\\" ) {
            $char = _consume_char( $state, 1 ) // '<end-of-input>';
            if ( $char eq '(' or $char eq ')'
                 or $char eq "\\" or $char =~ /\s/ ) {
                $token .= $char;
            }
            else {
                die qq{Tag expression "$state->{text}" could not be parsed because of syntax error: Illegal escape before "$char".};
            }
        }
        else {
            $token .= $char;
        }
    }
}

sub _save_token {
    my ( $state, $token ) = @_;

    $state->{saved_token} = $token;
}

sub _term_expr {
    my ( $state ) = @_;

    my $token = _get_token( $state );

    die qq{Tag expression "$state->{text}" could not be parsed because of syntax error: Expected operand.}
        if not defined $token;

    if ( $token eq '(' ) {
        my $expr = _expr( $state );
        my $token = _get_token( $state );

        if ( not $token or $token ne ')' ) {
            die qq{Tag expression "$state->{text}" could not be parsed because of syntax error: Unmatched (.}
        }

        return $expr;
    }
    elsif ( $token eq 'not' ) {
        return Cucumber::TagExpressions::NotNode->new(
            expression => _term_expr( $state )
            );
    }
    else {
        if ( $token eq 'and' or $token eq 'or' or $token eq 'not' ) {
            die qq{Tag expression "$state->{text}" could not be parsed because of syntax error: Expected operand."};
        }
        return Cucumber::TagExpressions::LiteralNode->new( tag => $token );
    }
}

sub _expr {
    my ( $state ) = @_;

    my @terms = ( _term_expr( $state ) );
    while ( my $token = _get_token( $state ) ) {
        if ( not defined $token or $token eq ')' ) {
            _save_token( $state, $token );
            last;
        }
        if ( not ( $token eq 'or'
                   or $token eq 'and' ) ) {
            die qq{Tag expression "$state->{text}" could not be parsed because of syntax error: Expected operator.}
        }

        my $term = _term_expr( $state );
        if ( $token eq 'and' ) {
            # immediately combine _and_ terms
            push @terms,
                Cucumber::TagExpressions::AndNode->new(
                    terms => [ pop(@terms), $term ]
                );
        }
        else {
            # collect _or_ terms
            push @terms, $term;
        }
    }

    if ( scalar(@terms) > 1 ) {
        return Cucumber::TagExpressions::OrNode->new(
            terms => \@terms
            );
    }
    # don't wrap a single-term expression in an Or node
    return $terms[0];
}

=head2 $class->parse( $expression )

Parses the string specified in C<$expression> and returns a
L<Cucumber::TagExpressions::ExpressionNode> instance.

=cut

sub parse {
    my ( $class, $text ) = @_;

    return Cucumber::TagExpressions::ExpressionNode->new(
        sub_expression => undef
        )
        if $text =~ /^\s*$/; # match the empty string or space-only string as "constant true"
    my $state = { pos => 0, text => $text, saved_token => undef };
    my $expr  = _expr( $state );

    my $token = _get_token( $state );

    if ( defined $token ) {
        if ( $token eq ')' ) {
            die qq{Tag expression "$state->{text}" could not be parsed because of syntax error: Unmatched ).};
        }

        die "Junk at end of expression: $token";
    }

    return Cucumber::TagExpressions::ExpressionNode->new(
        sub_expression => $expr
        );
}

1;

__END__

=head1 LICENSE

Please see the included LICENSE for the canonical version. In summary:

The MIT License (MIT)

  Copyright (c) 2021 Erik Huelsmann
  Copyright (c) 2021 Cucumber Ltd

This work is loosely derived from prior work of the same library for Ruby,
called C<cucumber-tag-expressions>.

=cut

